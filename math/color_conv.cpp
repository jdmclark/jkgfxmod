#include "color_conv.hpp"

namespace {
    float srgb_to_linear_em(float em)
    {
        if(em <= 0.04045f) {
            return em / 12.92f;
        }

        return std::pow((em + 0.055f) / 1.055f, 2.4f);
    }
}

jkgm::color jkgm::srgb_to_linear(color input)
{
    return color(srgb_to_linear_em(get<r>(input)),
                 srgb_to_linear_em(get<g>(input)),
                 srgb_to_linear_em(get<b>(input)),
                 get<a>(input));
}