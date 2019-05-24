#include "renderer_ao.hpp"
#include "common/config.hpp"
#include "renderer_ao_basic.hpp"
#include "renderer_ao_ssao.hpp"

std::unique_ptr<jkgm::renderer_ao> jkgm::create_renderer_ao(config const *the_config,
                                                            size<2, int> screen_res)
{
    if(the_config->enable_ssao) {
        return std::make_unique<renderer_ao_ssao>(screen_res);
    }

    return std::make_unique<renderer_ao_basic>();
}