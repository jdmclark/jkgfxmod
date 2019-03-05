#include "renderbuffer.hpp"
#include "glad/gl.h"

GLuint jkgm::gl::renderbuffer_traits::create()
{
    GLuint rv = 0;
    glGenRenderbuffers(1, &rv);
    return rv;
}

void jkgm::gl::renderbuffer_traits::destroy(GLuint id)
{
    glDeleteRenderbuffers(1, &id);
}

void jkgm::gl::bind_renderbuffer(renderbuffer_view buf)
{
    glBindRenderbuffer(GL_RENDERBUFFER, *buf);
}

void jkgm::gl::renderbuffer_storage(renderbuffer_format fmt, size<2, int> dimensions)
{
    glRenderbufferStorage(
        GL_RENDERBUFFER, static_cast<GLenum>(fmt), get<x>(dimensions), get<y>(dimensions));
}

void jkgm::gl::renderbuffer_storage_multisample(size_type num_samples,
                                               renderbuffer_format fmt,
                                               size<2, int> dimensions)
{
    glRenderbufferStorageMultisample(GL_RENDERBUFFER,
                                     num_samples,
                                     static_cast<GLenum>(fmt),
                                     get<x>(dimensions),
                                     get<y>(dimensions));
}

namespace jkgm::gl {
    static_assert(renderbuffer_format::depth == renderbuffer_format(GL_DEPTH_COMPONENT));
    static_assert(renderbuffer_format::stencil == renderbuffer_format(GL_STENCIL_INDEX));
}
