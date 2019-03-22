#include "triangle_batch.hpp"
#include "base/log.hpp"

jkgm::triangle_vertex::triangle_vertex()
    : pos(point<4, float>::zero())
    , texcoords(point<2, float>::zero())
    , color(jkgm::color::zero())
{
}

jkgm::triangle_vertex::triangle_vertex(point<4, float> pos,
                                       point<2, float> texcoords,
                                       jkgm::color color)
    : pos(pos)
    , texcoords(texcoords)
    , color(color)
{
}

jkgm::triangle::triangle()
    : normal(direction<3, float>::zero())
{
}

jkgm::triangle::triangle(triangle_vertex v0,
                         triangle_vertex v1,
                         triangle_vertex v2,
                         material_instance_id material)
    : v0(v0)
    , v1(v1)
    , v2(v2)
    , material(material)
    , normal(direction<3, float>::zero())
{
    auto p0 = make_point(get<x>(v0.pos), get<y>(v0.pos), get<w>(v0.pos));
    auto p1 = make_point(get<x>(v1.pos), get<y>(v1.pos), get<w>(v1.pos));
    auto p2 = make_point(get<x>(v2.pos), get<y>(v2.pos), get<w>(v2.pos));

    normal = normalize(cross(p1 - p0, p2 - p0));

    // JK disables backface culling. Make all of the normals point in the same direction.
    if(dot(normal, point<3, float>::zero() - p0) < 0.0f) {
        normal = -normal;
    }
}

jkgm::triangle_batch::triangle_batch()
{
    expand();
}

void jkgm::triangle_batch::expand()
{
    buffer.resize(buffer.size() + 10000U);
}

size_t jkgm::triangle_batch::size() const
{
    return num_triangles;
}

size_t jkgm::triangle_batch::capacity() const
{
    return buffer.size();
}

void jkgm::triangle_batch::clear()
{
    num_triangles = 0U;
}

void jkgm::triangle_batch::insert(triangle const &tri)
{
    if(num_triangles == buffer.size()) {
        expand();
    }

    buffer[num_triangles++] = tri;
}

void jkgm::triangle_batch::sort()
{
    std::sort(begin(), end(), [](auto const &a, auto const &b) {
        return a.material.get() < b.material.get();
    });
}

namespace jkgm {
    namespace {
        constexpr float dist_threshold = 0.001f;

        template <class ItT>
        void do_partition(ItT it_begin, ItT it_end)
        {
            if(it_begin == it_end) {
                return;
            }

            // Separate triangles into occluded-by, ambiguous, and occludes
            auto &a = *it_begin;
            auto a_v0 = get<xyw>(a.v0.pos);
            auto a_v1 = get<xyw>(a.v1.pos);
            auto a_v2 = get<xyw>(a.v2.pos);

            auto a_v0v1_nrm = normalize(cross(a_v1 - a_v0, a.normal));
            auto a_v1v2_nrm = normalize(cross(a_v2 - a_v1, a.normal));
            auto a_v2v0_nrm = normalize(cross(a_v0 - a_v2, a.normal));

            a.num_sup = 0;

            for(auto it = it_begin + 1; it != it_end; ++it) {
                auto &b = *it;
                auto b_v0 = get<xyw>(b.v0.pos);
                auto b_v1 = get<xyw>(b.v1.pos);
                auto b_v2 = get<xyw>(b.v2.pos);

                bool b_v0_inside = (dot(b_v0 - a_v0, a_v0v1_nrm) > 0.0f) &&
                                   (dot(b_v0 - a_v1, a_v1v2_nrm) > 0.0f) &&
                                   (dot(b_v0 - a_v2, a_v2v0_nrm) > 0.0f);
                auto b_v0_dist = dot(b_v0 - a_v0, a.normal);

                bool b_v1_inside = (dot(b_v1 - a_v0, a_v0v1_nrm) > 0.0f) &&
                                   (dot(b_v1 - a_v1, a_v1v2_nrm) > 0.0f) &&
                                   (dot(b_v1 - a_v2, a_v2v0_nrm) > 0.0f);
                auto b_v1_dist = dot(b_v1 - a_v0, a.normal);

                bool b_v2_inside = (dot(b_v2 - a_v0, a_v0v1_nrm) > 0.0f) &&
                                   (dot(b_v2 - a_v1, a_v1v2_nrm) > 0.0f) &&
                                   (dot(b_v2 - a_v2, a_v2v0_nrm) > 0.0f);
                auto b_v2_dist = dot(b_v2 - a_v0, a.normal);

                bool behind = !(b_v0_inside && b_v0_dist < dist_threshold) &&
                              !(b_v1_inside && b_v1_dist < dist_threshold) &&
                              !(b_v2_inside && b_v2_dist < dist_threshold);

                bool in_front = !(b_v0_inside && b_v0_dist > -dist_threshold) &&
                                !(b_v1_inside && b_v1_dist > -dist_threshold) &&
                                !(b_v2_inside && b_v2_dist > -dist_threshold);

                if(in_front) {
                    b.num_sup = -1;
                }
                else if(behind) {
                    b.num_sup = 1;
                }
                else {
                    b.num_sup = 0;
                }
            }

            std::sort(it_begin, it_end, [](auto const &a, auto const &b) {
                return a.num_sup < b.num_sup;
            });

            // Process each partition in order
            auto split_it = it_begin;
            for(; split_it != it_end; ++split_it) {
                if(split_it->num_sup != -1) {
                    break;
                }
            }

            auto split_jt = split_it;
            for(; split_jt != it_end; ++split_jt) {
                if(split_jt->num_sup != 0) {
                    break;
                }
            }

            int segs_remaining = ((it_begin != split_it) ? 1 : 0) +
                                 ((split_it != split_jt) ? 1 : 0) + ((split_jt != it_end) ? 1 : 0);

            if(segs_remaining <= 1) {
                // All remaining triangles are ambiguous with this pivot.
                do_partition(it_begin + 1, it_end);
                return;
            }

            do_partition(it_begin, split_it);
            do_partition(split_it, split_jt);
            do_partition(split_jt, it_end);
        }

        template <class ItT>
        void do_mid_partition(ItT it_begin, ItT it_end)
        {
            if(it_begin == it_end) {
                return;
            }

            // Partition triangles by a triangle
            auto &a = *it_begin;
            auto a_v0 = get<xyw>(a.v0.pos);
            auto a_v1 = get<xyw>(a.v1.pos);
            auto a_v2 = get<xyw>(a.v2.pos);

            a.num_sup = 0;

            for(auto it = it_begin + 1; it != it_end; ++it) {
                auto &b = *it;
                auto b_v0 = get<xyw>(b.v0.pos);
                auto b_v1 = get<xyw>(b.v1.pos);
                auto b_v2 = get<xyw>(b.v2.pos);

                auto b_v0_dist = dot(b_v0 - a_v0, a.normal);
                auto b_v1_dist = dot(b_v1 - a_v0, a.normal);
                auto b_v2_dist = dot(b_v2 - a_v0, a.normal);

                bool behind = (b_v0_dist < dist_threshold && b_v1_dist < dist_threshold &&
                               b_v2_dist < dist_threshold);
                bool in_front = (b_v0_dist > -dist_threshold && b_v1_dist > -dist_threshold &&
                                 b_v2_dist > -dist_threshold);

                if(in_front) {
                    b.num_sup = 1;
                }
                else if(behind) {
                    b.num_sup = -1;
                }
                else {
                    b.num_sup = 0;
                }
            }

            std::sort(it_begin, it_end, [](auto const &a, auto const &b) {
                return a.num_sup < b.num_sup;
            });

            // Process each partition in order
            auto split_it = it_begin;
            for(; split_it != it_end; ++split_it) {
                if(split_it->num_sup != -1) {
                    break;
                }
            }

            auto split_jt = split_it;
            for(; split_jt != it_end; ++split_jt) {
                if(split_jt->num_sup != 0) {
                    break;
                }
            }

            int segs_remaining = ((it_begin != split_it) ? 1 : 0) +
                                 ((split_it != split_jt) ? 1 : 0) + ((split_jt != it_end) ? 1 : 0);

            if(segs_remaining <= 1) {
                // All remaining triangles are ambiguous with this pivot.
                do_partition(it_begin, it_end);
                return;
            }

            do_mid_partition(it_begin, split_it);
            do_partition(split_it, split_jt);
            do_mid_partition(split_jt, it_end);
        }

        template <class ItT>
        void do_fast_partition(ItT it_begin, ItT it_end)
        {
            if(it_begin == it_end) {
                return;
            }

            // Partition triangles by the midpoint of the space.
            // Note that the eye space depth coordinate is stored in W, not in Z.
            float space_start = std::numeric_limits<float>::max();
            float space_end = std::numeric_limits<float>::lowest();
            for(auto it = it_begin; it != it_end; ++it) {
                auto const &em = *it;

                auto z0 = get<w>(em.v0.pos);
                auto z1 = get<w>(em.v1.pos);
                auto z2 = get<w>(em.v2.pos);

                space_start = std::min(space_start, std::min(z0, std::min(z1, z2)));
                space_end = std::max(space_start, std::max(z0, std::max(z1, z2)));
            }

            float space_midpoint = (space_start + space_end) * 0.5f;
            float space_midpoint_near = space_midpoint + dist_threshold;
            float space_midpoint_far = space_midpoint - dist_threshold;

            for(auto it = it_begin; it != it_end; ++it) {
                auto &em = *it;

                auto z0 = get<w>(em.v0.pos);
                auto z1 = get<w>(em.v1.pos);
                auto z2 = get<w>(em.v2.pos);

                bool is_near =
                    (z0 > space_midpoint_far && z1 > space_midpoint_far && z2 > space_midpoint_far);
                bool is_far = (z0 < space_midpoint_near && z1 < space_midpoint_near &&
                               z2 < space_midpoint_near);

                if(is_near && is_far) {
                    em.num_sup = 0;
                }
                else if(is_near) {
                    em.num_sup = -1;
                }
                else if(is_far) {
                    em.num_sup = 1;
                }
                else {
                    em.num_sup = 0;
                }
            }

            std::sort(it_begin, it_end, [](auto const &a, auto const &b) {
                return a.num_sup < b.num_sup;
            });

            auto split_it = it_begin;
            for(; split_it != it_end; ++split_it) {
                if(split_it->num_sup != -1) {
                    break;
                }
            }

            auto split_jt = split_it;
            for(; split_jt != it_end; ++split_jt) {
                if(split_jt->num_sup != 0) {
                    break;
                }
            }

            int segs_remaining = ((it_begin != split_it) ? 1 : 0) +
                                 ((split_it != split_jt) ? 1 : 0) + ((split_jt != it_end) ? 1 : 0);

            if(segs_remaining <= 1) {
                // Remaining triangles can't be separated this way
                do_mid_partition(it_begin, it_end);
                return;
            }

            do_fast_partition(it_begin, split_it);
            do_mid_partition(split_it, split_jt);
            do_fast_partition(split_jt, it_end);
        }
    }
}

void jkgm::sorted_triangle_batch::sort()
{
    do_fast_partition(begin(), end());
}