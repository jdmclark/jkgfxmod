#include "image.hpp"
#include "base/log.hpp"
#include "base/memory_block.hpp"
#include "stb_image.h"
#include "stb_image_write.h"

jkgm::image::image(size<2, int> dimensions)
    : dimensions(dimensions)
{
    data.resize(volume(dimensions), color_rgba8::zero());
}

std::unique_ptr<jkgm::image> jkgm::load_image(input_stream *is)
{
    memory_block mb;
    memory_output_block mob(&mb);
    is->copy_to(&mob);

    auto dat_span = make_span(mb).as_unsigned_const_bytes();

    int width, height, channels;
    unsigned char *data = stbi_load_from_memory(
        dat_span.data(), dat_span.size(), &width, &height, &channels, STBI_rgb_alpha);

    if(data == nullptr) {
        LOG_ERROR("Error loading image");
        throw std::runtime_error("invalid image");
    }

    auto in_span = make_span(data, width * height * 4);

    auto rv = std::make_unique<image>(make_size(width, height));
    auto out_span = make_span(rv->data).as_unsigned_bytes();

    std::copy(in_span.begin(), in_span.end(), out_span.begin());

    stbi_image_free(data);

    return rv;
}

void jkgm::store_image_png(output_stream *os, image const &img)
{
    auto write_func = [](void *context, void *data, int size) {
        auto *os = (output_stream *)context;
        os->write(make_span((char const *)data, size));
    };

    int result = stbi_write_png_to_func(write_func,
                                        (void *)os,
                                        get<x>(img.dimensions),
                                        get<y>(img.dimensions),
                                        /*components*/ 4,
                                        img.data.data(),
                                        /*stride*/ get<x>(img.dimensions) * sizeof(color_rgba8));
}