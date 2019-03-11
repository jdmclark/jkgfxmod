#include "triangle_batch.hpp"

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
                         size_t material_index)
    : v0(v0)
    , v1(v1)
    , v2(v2)
    , material_index(material_index)
    , normal(direction<3, float>::zero())
{
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
        return a.material_index < b.material_index;
    });
}

void jkgm::sorted_triangle_batch::sort()
{
    if(buffer.size() < 2) {
        return;
    }

    // Precompute normals
    for(auto &em : *this) {
        auto p0 = get<xyz>(em.v0.pos);
        auto p1 = get<xyz>(em.v1.pos);
        auto p2 = get<xyz>(em.v2.pos);

        em.normal = normalize(cross(p1 - p0, p2 - p0));
    }

    std::sort(begin(), end(), [](triangle const &a, triangle const &b) {
        auto a_v0 = get<xyz>(a.v0.pos);
        auto a_v1 = get<xyz>(a.v1.pos);
        auto a_v2 = get<xyz>(a.v2.pos);
        auto b_v0 = get<xyz>(b.v0.pos);
        auto b_v1 = get<xyz>(b.v1.pos);
        auto b_v2 = get<xyz>(b.v2.pos);

        bool a_v0_sup = (dot(a_v0 - b_v0, b.normal) >= 0.0f);
        bool a_v1_sup = (dot(a_v1 - b_v0, b.normal) >= 0.0f);
        bool a_v2_sup = (dot(a_v2 - b_v0, b.normal) >= 0.0f);
        bool b_v0_sup = (dot(b_v0 - a_v0, a.normal) >= 0.0f);
        bool b_v1_sup = (dot(b_v1 - a_v0, a.normal) >= 0.0f);
        bool b_v2_sup = (dot(b_v2 - a_v0, a.normal) >= 0.0f);

        return (a_v0_sup || a_v1_sup || a_v2_sup) && !b_v0_sup && !b_v1_sup && !b_v2_sup;
    });
}