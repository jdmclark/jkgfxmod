#include "framebuffer.hpp"
#include "glad/gl.h"

GLuint jkgm::gl::framebuffer_traits::create()
{
    GLuint rv = 0;
    glGenFramebuffers(1, &rv);
    return rv;
}

void jkgm::gl::framebuffer_traits::destroy(GLuint id)
{
    glDeleteFramebuffers(1, &id);
}

void jkgm::gl::bind_framebuffer(framebuffer_bind_target target, framebuffer_view buf)
{
    glBindFramebuffer(static_cast<GLenum>(target), *buf);
}

void jkgm::gl::blit_framebuffer(box<2, int> src,
                                box<2, int> dest,
                                framebuffer_blit_mask mask,
                                framebuffer_blit_filter filter)
{
    glBlitFramebuffer(get<x>(src.start),
                      get<y>(src.start),
                      get<x>(src.stop),
                      get<y>(src.stop),
                      get<x>(dest.start),
                      get<y>(dest.start),
                      get<x>(dest.stop),
                      get<y>(dest.stop),
                      static_cast<GLbitfield>(mask),
                      static_cast<GLenum>(filter));
}

jkgm::gl::framebuffer_status jkgm::gl::check_framebuffer_status(framebuffer_bind_target target)
{
    return framebuffer_status(glCheckFramebufferStatus(static_cast<GLenum>(target)));
}

void jkgm::gl::clear_buffer_depth(float value)
{
    glClearBufferfv(GL_DEPTH, 0, &value);
}

void jkgm::gl::clear_buffer_color(int drawbuffer, color value)
{
    glClearBufferfv(GL_COLOR, drawbuffer, value.data.data());
}

void jkgm::gl::detail::draw_buffers_span(span<enum_type const> bufs)
{
    glDrawBuffers(bufs.size(), bufs.data());
}

void jkgm::gl::framebuffer_texture(framebuffer_bind_target target,
                                   framebuffer_attachment attachment,
                                   texture_view tex,
                                   int level)
{
    glFramebufferTexture(static_cast<GLenum>(target), static_cast<GLenum>(attachment), *tex, level);
}

void jkgm::gl::framebuffer_texture_2d(framebuffer_bind_target target,
                                      framebuffer_attachment attachment,
                                      texture_bind_target textarget,
                                      texture_view tex,
                                      int level)
{
    glFramebufferTexture2D(static_cast<GLenum>(target),
                           static_cast<GLenum>(attachment),
                           static_cast<GLenum>(textarget),
                           *tex,
                           level);
}

void jkgm::gl::framebuffer_renderbuffer(framebuffer_bind_target target,
                                        framebuffer_attachment attachment,
                                        renderbuffer_view buf)
{
    glFramebufferRenderbuffer(
        static_cast<GLenum>(target), static_cast<GLenum>(attachment), GL_RENDERBUFFER, *buf);
}

void jkgm::gl::read_buffer(framebuffer_attachment attachment)
{
    glReadBuffer(static_cast<GLenum>(attachment));
}

namespace jkgm::gl {
    static_assert(draw_buffer::none == draw_buffer(GL_NONE));
    static_assert(draw_buffer::front_left == draw_buffer(GL_FRONT_LEFT));
    static_assert(draw_buffer::front_right == draw_buffer(GL_FRONT_RIGHT));
    static_assert(draw_buffer::back_left == draw_buffer(GL_BACK_LEFT));
    static_assert(draw_buffer::back_right == draw_buffer(GL_BACK_RIGHT));
    static_assert(draw_buffer::color0 == draw_buffer(GL_COLOR_ATTACHMENT0));
    static_assert(draw_buffer::color1 == draw_buffer(GL_COLOR_ATTACHMENT1));
    static_assert(draw_buffer::color2 == draw_buffer(GL_COLOR_ATTACHMENT2));

    static_assert(framebuffer_attachment::color0 == framebuffer_attachment(GL_COLOR_ATTACHMENT0));
    static_assert(framebuffer_attachment::color1 == framebuffer_attachment(GL_COLOR_ATTACHMENT1));
    static_assert(framebuffer_attachment::color2 == framebuffer_attachment(GL_COLOR_ATTACHMENT2));
    static_assert(framebuffer_attachment::depth == framebuffer_attachment(GL_DEPTH_ATTACHMENT));
    static_assert(framebuffer_attachment::stencil == framebuffer_attachment(GL_STENCIL_ATTACHMENT));
    static_assert(framebuffer_attachment::depth_stencil ==
                  framebuffer_attachment(GL_DEPTH_STENCIL_ATTACHMENT));

    static_assert(framebuffer_bind_target::any == framebuffer_bind_target(GL_FRAMEBUFFER));
    static_assert(framebuffer_bind_target::draw == framebuffer_bind_target(GL_DRAW_FRAMEBUFFER));
    static_assert(framebuffer_bind_target::read == framebuffer_bind_target(GL_READ_FRAMEBUFFER));

    static_assert(framebuffer_blit_buffer::color == framebuffer_blit_buffer(GL_COLOR_BUFFER_BIT));
    static_assert(framebuffer_blit_buffer::depth == framebuffer_blit_buffer(GL_DEPTH_BUFFER_BIT));

    static_assert(framebuffer_blit_filter::linear == framebuffer_blit_filter(GL_LINEAR));
    static_assert(framebuffer_blit_filter::nearest == framebuffer_blit_filter(GL_NEAREST));

    static_assert(framebuffer_status::complete == framebuffer_status(GL_FRAMEBUFFER_COMPLETE));
    static_assert(framebuffer_status::undefined == framebuffer_status(GL_FRAMEBUFFER_UNDEFINED));
    static_assert(framebuffer_status::incomplete_attachment ==
                  framebuffer_status(GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT));
    static_assert(framebuffer_status::incomplete_missing_attachment ==
                  framebuffer_status(GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT));
    static_assert(framebuffer_status::incomplete_draw_buffer ==
                  framebuffer_status(GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER));
    static_assert(framebuffer_status::incomplete_read_buffer ==
                  framebuffer_status(GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER));
    static_assert(framebuffer_status::unsupported ==
                  framebuffer_status(GL_FRAMEBUFFER_UNSUPPORTED));
    static_assert(framebuffer_status::incomplete_multisample ==
                  framebuffer_status(GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE));
    static_assert(framebuffer_status::incomplete_layer_targets ==
                  framebuffer_status(GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS));
}
