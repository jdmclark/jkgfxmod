#include "renderer_ao_basic.hpp"

void jkgm::renderer_ao_basic::update_ssao_texture(opengl_state * /*ogs*/,
                                                  gl::texture_view /*normal_tex*/,
                                                  gl::texture_view /*depth_tex*/)
{
}

jkgm::gl::texture_view jkgm::renderer_ao_basic::get_ssao_texture() const
{
    return gl::default_texture;
}