#pragma once

#include "math/color.hpp"
#include "math/direction.hpp"
#include "math/point.hpp"
#include "renderer.hpp"
#include <vector>

namespace jkgm {
    struct triangle_vertex {
        point<4, float> pos;
        point<2, float> texcoords;
        jkgm::color color;

        triangle_vertex();
        triangle_vertex(point<4, float> pos, point<2, float> texcoords, jkgm::color color);
    };

    struct triangle {
        triangle_vertex v0, v1, v2;
        material_instance_id material = material_instance_id(0U);
        direction<3, float> normal;
        int num_sup = 0;

        triangle();
        triangle(triangle_vertex v0,
                 triangle_vertex v1,
                 triangle_vertex v2,
                 material_instance_id material);
    };

    class triangle_batch {
    protected:
        std::vector<triangle> buffer;
        size_t num_triangles = 0U;

        void expand();

    public:
        triangle_batch();
        virtual ~triangle_batch() = default;

        size_t size() const;
        size_t capacity() const;
        void clear();
        void insert(triangle const &tri);

        virtual void sort();

        inline auto begin() const
        {
            return buffer.begin();
        }

        inline auto end() const
        {
            return buffer.begin() + num_triangles;
        }

        inline auto begin()
        {
            return buffer.begin();
        }

        inline auto end()
        {
            return buffer.begin() + num_triangles;
        }
    };

    class sorted_triangle_batch : public triangle_batch {
    protected:
        std::vector<triangle> buffer;
        size_t num_triangles = 0U;

    public:
        void sort() override;
    };
}