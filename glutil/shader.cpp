#include "shader.hpp"
#include "glad/gl.h"
#include <array>

GLuint jkgm::gl::shader_traits::create(shader_type type)
{
    return glCreateShader(static_cast<GLenum>(type));
}

void jkgm::gl::shader_traits::destroy(GLuint id)
{
    glDeleteShader(id);
}

void jkgm::gl::shader_source(shader_view id, span<char const> prefix_text, span<char const> src_text)
{
    std::array<char const *, 2> src_ptrs = {prefix_text.data(), src_text.data()};
    std::array<int, 2> src_lens = {static_cast<int>(prefix_text.size()),
                                   static_cast<int>(src_text.size())};

    glShaderSource(*id, 2, src_ptrs.data(), src_lens.data());
}

void jkgm::gl::compile_shader(shader_view id)
{
    glCompileShader(*id);
}

bool jkgm::gl::get_shader_compile_status(shader_view id)
{
    GLint result = GL_FALSE;
    glGetShaderiv(*id, GL_COMPILE_STATUS, &result);

    return result != GL_FALSE;
}

std::string jkgm::gl::get_shader_info_log(shader_view id)
{
    std::string rv;

    int log_length = 0;
    glGetShaderiv(*id, GL_INFO_LOG_LENGTH, &log_length);
    if(log_length > 0) {
        rv.resize(log_length, 'X');
        glGetShaderInfoLog(*id, log_length, nullptr, rv.data());
    }

    return rv;
}

namespace jkgm::gl {
    static_assert(shader_type::vertex == shader_type(GL_VERTEX_SHADER));
    static_assert(shader_type::fragment == shader_type(GL_FRAGMENT_SHADER));
}
