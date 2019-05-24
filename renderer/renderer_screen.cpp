#include "renderer_screen.hpp"
#include "renderer_screen_basic.hpp"

std::unique_ptr<jkgm::renderer_screen> jkgm::create_renderer_screen(config const * /*the_config*/,
                                                                    size<2, int> screen_res)
{
    return std::make_unique<renderer_screen_basic>(screen_res);
}