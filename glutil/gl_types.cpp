#include "gl_types.hpp"
#include "glad/gl.h"
#include <type_traits>

namespace jkgm::gl {
    static_assert(std::is_same_v<enum_type, GLenum>, "GLenum type mismatch");
    static_assert(std::is_same_v<boolean_type, GLboolean>, "GLboolean type mismatch");
    static_assert(std::is_same_v<bitfield_type, GLbitfield>, "GLbitfield type mismatch");
    static_assert(std::is_same_v<byte_type, GLbyte>, "GLbyte type mismatch");
    static_assert(std::is_same_v<short_type, GLshort>, "GLshort type mismatch");
    static_assert(std::is_same_v<int_type, GLint>, "GLint type mismatch");
    static_assert(std::is_same_v<ubyte_type, GLubyte>, "GLubyte type mismatch");
    static_assert(std::is_same_v<ushort_type, GLushort>, "GLushort type mismatch");
    static_assert(std::is_same_v<uint_type, GLuint>, "GLuint type mismatch");
    static_assert(std::is_same_v<size_type, GLsizei>, "GLsizei type mismatch");

    static_assert(false_value == GL_FALSE, "GL_FALSE value mismatch");
    static_assert(true_value == GL_TRUE, "GL_TRUE value mismatch");
}
