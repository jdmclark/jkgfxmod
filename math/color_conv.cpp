#include "color_conv.hpp"
#include "base/log.hpp"
#include <array>

namespace {
    float srgb_to_linear_em(float em)
    {
        if(em <= 0.04045f) {
            return em / 12.92f;
        }

        return std::pow((em + 0.055f) / 1.055f, 2.4f);
    }

    // Precalculated lookup tables for converting sRGB565 to linear RGB888
    std::array<uint8_t, 32> linear_5bit_lut{{0,   0,   1,   2,   3,   5,   7,   10,  13,  17, 21,
                                             26,  31,  37,  43,  50,  58,  66,  75,  85,  95, 106,
                                             117, 130, 143, 156, 171, 186, 202, 219, 236, 255}};
    std::array<uint8_t, 64> linear_6bit_lut{
        {0,   0,   0,   0,   1,   1,   2,   3,   3,   4,   5,   6,   7,   8,   10,  11,
         13,  15,  16,  18,  20,  23,  25,  27,  30,  33,  36,  39,  42,  45,  49,  52,
         56,  60,  64,  68,  72,  77,  82,  87,  92,  97,  102, 107, 113, 119, 125, 131,
         138, 144, 151, 158, 165, 172, 179, 187, 195, 203, 211, 219, 228, 236, 245, 255}};
}

jkgm::color jkgm::srgb_to_linear(color input)
{
    return color(srgb_to_linear_em(get<r>(input)),
                 srgb_to_linear_em(get<g>(input)),
                 srgb_to_linear_em(get<b>(input)),
                 get<a>(input));
}

jkgm::color_rgba8 jkgm::rgb565_key_to_linear(uint16_t input, bool transparent)
{
    if(transparent) {
        return color_rgba8::zero();
    }

    return color_rgba8(linear_5bit_lut[(input >> 11) & 0x1F],
                       linear_6bit_lut[(input >> 5) & 0x3F],
                       linear_5bit_lut[input & 0x1F],
                       (transparent ? uint8_t(0) : uint8_t(255)));
}

jkgm::color_rgba8 jkgm::rgb565_to_linear(uint16_t input)
{
    float r = float((input >> 11) & 0x1F) / float(0x1F);
    float g = float((input >> 5) & 0x3F) / float(0x3F);
    float b = float((input >> 0) & 0x1F) / float(0x1F);

    return to_discrete_color(srgb_to_linear(color(r, g, b, 1.0f)));
}

jkgm::color_rgba8 jkgm::rgba5551_to_linear(uint16_t input)
{
    float a = float((input >> 15) & 0x1) / float(0x1);
    float r = float((input >> 10) & 0x1F) / float(0x1F);
    float g = float((input >> 5) & 0x1F) / float(0x1F);
    float b = float((input >> 0) & 0x1F) / float(0x1F);

    return to_discrete_color(srgb_to_linear(color(r * a, g * a, b * a, a)));
}