#include "vertex_array.hpp"
#include "glad/gl.h"

GLuint jkgm::gl::vertex_array_traits::create()
{
    GLuint rv = 0;
    glGenVertexArrays(1, &rv);
    return rv;
}

void jkgm::gl::vertex_array_traits::destroy(GLuint id)
{
    glDeleteVertexArrays(1, &id);
}

void jkgm::gl::bind_vertex_array(vertex_array_view vao)
{
    glBindVertexArray(*vao);
}

void jkgm::gl::enable_vertex_attrib_array(GLuint index)
{
    glEnableVertexAttribArray(index);
}

void jkgm::gl::vertex_attrib_pointer(GLuint index,
                                     GLint num_components,
                                     vertex_element_type type,
                                     bool normalized,
                                     size_t stride,
                                     ptrdiff_t offset)
{
    glVertexAttribPointer(index,
                          num_components,
                          static_cast<GLenum>(type),
                          normalized ? GL_TRUE : GL_FALSE,
                          stride,
                          reinterpret_cast<GLvoid const *>(offset));
}

void jkgm::gl::draw_elements(element_type type,
                             size_t num_indices,
                             index_type itype,
                             ptrdiff_t offset)
{
    glDrawElements(static_cast<GLenum>(type),
                   num_indices,
                   static_cast<GLenum>(itype),
                   reinterpret_cast<GLvoid const *>(offset));
}

void jkgm::gl::draw_arrays(element_type type, size_t offset, size_t count)
{
    glDrawArrays(static_cast<GLenum>(type), offset, count);
}

namespace jkgm::gl {
    static_assert(vertex_element_type::int8 == vertex_element_type(GL_BYTE));
    static_assert(vertex_element_type::uint8 == vertex_element_type(GL_UNSIGNED_BYTE));
    static_assert(vertex_element_type::int16 == vertex_element_type(GL_SHORT));
    static_assert(vertex_element_type::uint16 == vertex_element_type(GL_UNSIGNED_SHORT));
    static_assert(vertex_element_type::int32 == vertex_element_type(GL_INT));
    static_assert(vertex_element_type::uint32 == vertex_element_type(GL_UNSIGNED_INT));
    static_assert(vertex_element_type::float32 == vertex_element_type(GL_FLOAT));

    static_assert(element_type::triangles == element_type(GL_TRIANGLES));

    static_assert(index_type::uint8 == index_type(GL_UNSIGNED_BYTE));
    static_assert(index_type::uint16 == index_type(GL_UNSIGNED_SHORT));
    static_assert(index_type::uint32 == index_type(GL_UNSIGNED_INT));
}
