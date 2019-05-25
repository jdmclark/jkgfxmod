#include "color_conv.hpp"
#include "base/log.hpp"
#include <array>

namespace {
    float gamma18_to_linear_em(float em)
    {
        return std::pow(em, 1.8f);
    }

    float srgb_to_linear_em(float em)
    {
        if(em <= 0.04045f) {
            return em / 12.92f;
        }

        return std::pow((em + 0.055f) / 1.055f, 2.4f);
    }

    float linear_to_srgb_em(float em)
    {
        if(em > 0.0031308f) {
            return 1.055f * (pow(em, (1.0f / 2.4f))) - 0.055f;
        }

        return 12.92f * em;
    }

    // Precalculated lookup tables for converting sRGB565 to sRGB888
    std::array<uint8_t, 32> srgb_5bit_lut{{0,   8,   16,  25,  33,  41,  49,  58,  66,  74,  82,
                                           90,  99,  107, 115, 123, 132, 140, 148, 156, 165, 173,
                                           181, 189, 197, 206, 214, 222, 230, 239, 247, 255}};

    std::array<uint8_t, 64> srgb_6bit_lut{
        {0,   4,   8,   12,  16,  20,  24,  28,  32,  36,  40,  45,  49,  53,  57,  61,
         65,  69,  73,  77,  81,  85,  89,  93,  97,  101, 105, 109, 113, 117, 121, 125,
         130, 134, 138, 142, 146, 150, 154, 158, 162, 166, 170, 174, 178, 182, 186, 190,
         194, 198, 202, 206, 210, 215, 219, 223, 227, 231, 235, 239, 243, 247, 251, 255}};
}

jkgm::color jkgm::gamma18_to_linear(color input)
{
    return color(gamma18_to_linear_em(get<r>(input)),
                 gamma18_to_linear_em(get<g>(input)),
                 gamma18_to_linear_em(get<b>(input)),
                 get<a>(input));
}

jkgm::color jkgm::srgb_to_linear(color input)
{
    return color(srgb_to_linear_em(get<r>(input)),
                 srgb_to_linear_em(get<g>(input)),
                 srgb_to_linear_em(get<b>(input)),
                 get<a>(input));
}

jkgm::color jkgm::linear_to_srgb(color input)
{
    return color(linear_to_srgb_em(get<r>(input)),
                 linear_to_srgb_em(get<g>(input)),
                 linear_to_srgb_em(get<b>(input)),
                 get<a>(input));
}

jkgm::color_rgba8 jkgm::rgb565_key_to_srgb_a8(uint16_t input, bool transparent)
{
    if(transparent) {
        return color_rgba8::zero();
    }

    return color_rgba8(srgb_5bit_lut[(input >> 11) & 0x1F],
                       srgb_6bit_lut[(input >> 5) & 0x3F],
                       srgb_5bit_lut[input & 0x1F],
                       uint8_t(255));
}

jkgm::color_rgba8 jkgm::rgb565_to_srgb_a8(uint16_t input)
{
    float r = float((input >> 11) & 0x1F) / float(0x1F);
    float g = float((input >> 5) & 0x3F) / float(0x3F);
    float b = float((input >> 0) & 0x1F) / float(0x1F);

    return to_discrete_color(color(r, g, b, 1.0f));
}

jkgm::color_rgba8 jkgm::rgba5551_to_srgb_a8(uint16_t input)
{
    float a = float((input >> 15) & 0x1) / float(0x1);
    float r = float((input >> 10) & 0x1F) / float(0x1F);
    float g = float((input >> 5) & 0x1F) / float(0x1F);
    float b = float((input >> 0) & 0x1F) / float(0x1F);

    auto s_col = srgb_to_linear(color(r, g, b, a));
    auto pm_col = extend(get<rgb>(s_col) * a, a);
    return to_discrete_color(linear_to_srgb(pm_col));
}

uint16_t jkgm::srgb_a8_to_rgb565(color_rgba8 input)
{
    auto r = (uint16_t)(((get<0>(input) >> 3) & 0x1F) << 11);
    auto g = (uint16_t)(((get<1>(input) >> 2) & 0x3F) << 5);
    auto b = (uint16_t)((get<2>(input) >> 3) & 0x1F);

    return r | g | b;
}

uint16_t jkgm::srgb_a8_to_rgba5551(color_rgba8 input)
{
    auto a = (uint16_t)(((get<3>(input) >> 7) & 0x1) << 15);
    auto r = (uint16_t)(((get<0>(input) >> 3) & 0x1F) << 10);
    auto g = (uint16_t)(((get<1>(input) >> 3) & 0x1F) << 5);
    auto b = (uint16_t)((get<2>(input) >> 3) & 0x1F);

    return a | r | g | b;
}