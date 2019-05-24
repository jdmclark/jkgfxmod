#include "gl.hpp"
#include "base/log.hpp"
#include "glad/gl.h"

void jkgm::gl::enable(capability cap)
{
    glEnable(static_cast<GLenum>(cap));
}

void jkgm::gl::disable(capability cap)
{
    glDisable(static_cast<GLenum>(cap));
}

void jkgm::gl::clear(clear_flags cf)
{
    glClear(static_cast<GLbitfield>(cf));
}

void jkgm::gl::set_blend_function(blend_function sfactor, blend_function dfactor)
{
    glBlendFunc(static_cast<GLenum>(sfactor), static_cast<GLenum>(dfactor));
}

void jkgm::gl::set_clear_color(color c)
{
    glClearColor(get<r>(c), get<g>(c), get<b>(c), get<a>(c));
}

void jkgm::gl::set_depth_function(comparison_function func)
{
    glDepthFunc(static_cast<GLenum>(func));
}

void jkgm::gl::set_depth_mask(bool enable)
{
    glDepthMask(enable ? GL_TRUE : GL_FALSE);
}

void jkgm::gl::set_face_cull_mode(face_mode mode)
{
    glCullFace(static_cast<GLenum>(mode));
}

void jkgm::gl::set_min_sample_shading_factor(float f)
{
    glMinSampleShadingARB(f);
}

void jkgm::gl::set_polygon_mode(face_mode fm, polygon_mode pm)
{
    glPolygonMode(static_cast<GLenum>(fm), static_cast<GLenum>(pm));
}

void jkgm::gl::set_viewport(box<2, int> vp)
{
    auto dim = vp.stop - vp.start;
    glViewport(get<x>(vp.start), get<y>(vp.start), get<x>(dim), get<y>(dim));
}

void jkgm::gl::log_errors()
{
    for(;;) {
        auto e = glGetError();
        if(e == GL_NO_ERROR) {
            break;
        }

        LOG_ERROR("OpenGL error: ", hex(static_cast<size_t>(e)));
    }
}

namespace jkgm::gl {
    static_assert(blend_function::zero == blend_function(GL_ZERO));
    static_assert(blend_function::one == blend_function(GL_ONE));
    static_assert(blend_function::one_minus_source_alpha == blend_function(GL_ONE_MINUS_SRC_ALPHA));

    static_assert(capability::blend == capability(GL_BLEND));
    static_assert(capability::cull_face == capability(GL_CULL_FACE));
    static_assert(capability::depth_test == capability(GL_DEPTH_TEST));
    static_assert(capability::framebuffer_srgb == capability(GL_FRAMEBUFFER_SRGB));
    static_assert(capability::multisample == capability(GL_MULTISAMPLE));
    static_assert(capability::sample_shading == capability(GL_SAMPLE_SHADING_ARB));

    static_assert(clear_flag::color == clear_flag(GL_COLOR_BUFFER_BIT));
    static_assert(clear_flag::depth == clear_flag(GL_DEPTH_BUFFER_BIT));

    static_assert(comparison_function::never == comparison_function(GL_NEVER));
    static_assert(comparison_function::less == comparison_function(GL_LESS));
    static_assert(comparison_function::equal == comparison_function(GL_EQUAL));
    static_assert(comparison_function::less_equal == comparison_function(GL_LEQUAL));
    static_assert(comparison_function::greater == comparison_function(GL_GREATER));
    static_assert(comparison_function::not_equal == comparison_function(GL_NOTEQUAL));
    static_assert(comparison_function::greater_equal == comparison_function(GL_GEQUAL));
    static_assert(comparison_function::always == comparison_function(GL_ALWAYS));

    static_assert(face_mode::front == face_mode(GL_FRONT));
    static_assert(face_mode::back == face_mode(GL_BACK));
    static_assert(face_mode::front_and_back == face_mode(GL_FRONT_AND_BACK));

    static_assert(polygon_mode::point == polygon_mode(GL_POINT));
    static_assert(polygon_mode::line == polygon_mode(GL_LINE));
    static_assert(polygon_mode::fill == polygon_mode(GL_FILL));
}
