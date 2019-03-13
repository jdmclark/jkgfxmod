#pragma once

#include "base/input_stream.hpp"
#include "base/output_stream.hpp"
#include "math/color.hpp"
#include "math/size.hpp"
#include <vector>

namespace jkgm {
    class image {
    public:
        size<2, int> dimensions;
        std::vector<color_rgba8> data;

        explicit image(size<2, int> dimensions);
    };

    std::unique_ptr<image> load_image(input_stream *is);
    void store_image_png(output_stream *os, image const &img);
}