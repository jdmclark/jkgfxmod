#include "buffer.hpp"
#include "glad/gl.h"

GLuint jkgm::gl::buffer_traits::create()
{
    GLuint rv = 0;
    glGenBuffers(1, &rv);
    return rv;
}

void jkgm::gl::buffer_traits::destroy(GLuint id)
{
    glDeleteBuffers(1, &id);
}

void jkgm::gl::bind_buffer(buffer_bind_target target, buffer_view buf)
{
    glBindBuffer(static_cast<GLenum>(target), *buf);
}

void jkgm::gl::buffer_reserve(buffer_bind_target target, size_t size, buffer_usage usage)
{
    glBufferData(static_cast<GLenum>(target), size, nullptr, static_cast<GLenum>(usage));
}

void jkgm::gl::buffer_data(buffer_bind_target target, span<char const> data, buffer_usage usage)
{
    glBufferData(static_cast<GLenum>(target), data.size(), data.data(), static_cast<GLenum>(usage));
}

void jkgm::gl::buffer_sub_data(buffer_bind_target target, size_t offset, span<char const> data)
{
    glBufferSubData(static_cast<GLenum>(target), offset, data.size(), data.data());
}

namespace jkgm::gl {
    static_assert(buffer_bind_target::array == buffer_bind_target(GL_ARRAY_BUFFER));
    static_assert(buffer_bind_target::element_array == buffer_bind_target(GL_ELEMENT_ARRAY_BUFFER));

    static_assert(buffer_usage::static_draw == buffer_usage(GL_STATIC_DRAW));
    static_assert(buffer_usage::dynamic_draw == buffer_usage(GL_DYNAMIC_DRAW));
}
