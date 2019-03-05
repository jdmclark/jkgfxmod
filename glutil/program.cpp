#include "program.hpp"
#include "glad/gl.h"

GLuint jkgm::gl::program_traits::create()
{
    return glCreateProgram();
}

void jkgm::gl::program_traits::destroy(GLuint id)
{
    glDeleteProgram(id);
}

void jkgm::gl::attach_shader(program_view id, shader_view s)
{
    glAttachShader(*id, *s);
}

void jkgm::gl::link_program(program_view id)
{
    glLinkProgram(*id);
}

bool jkgm::gl::get_program_link_status(program_view id)
{
    GLint result = GL_FALSE;
    glGetProgramiv(*id, GL_LINK_STATUS, &result);

    return result != GL_FALSE;
}

std::string jkgm::gl::get_program_info_log(program_view id)
{
    std::string rv;

    int log_length = 0;
    glGetProgramiv(*id, GL_INFO_LOG_LENGTH, &log_length);
    if(log_length > 0) {
        rv.resize(log_length, 'X');
        glGetProgramInfoLog(*id, log_length, nullptr, rv.data());
    }

    return rv;
}

void jkgm::gl::use_program(program_view prog)
{
    glUseProgram(*prog);
}

jkgm::gl::uniform_location_id jkgm::gl::get_uniform_location(program_view prog, cstring_view name)
{
    return uniform_location_id(glGetUniformLocation(*prog, name.c_str()));
}

void jkgm::gl::detail::inner_set_uniform_matrix_4fv(uniform_location_id uniform,
                                                   bool transpose,
                                                   std::array<float, 16> const &value)
{
    glUniformMatrix4fv(uniform.get(), /*count*/ 1U, to_gl_bool(transpose), value.data());
}

void jkgm::gl::set_uniform_integer(uniform_location_id uniform, int value)
{
    glUniform1i(uniform.get(), value);
}

void jkgm::gl::set_uniform_float(uniform_location_id uniform, float value)
{
    glUniform1f(uniform.get(), value);
}

void jkgm::gl::detail::inner_set_uniform_vec(uniform_location_id uniform, int v0)
{
    glUniform1i(uniform.get(), v0);
}

void jkgm::gl::detail::inner_set_uniform_vec(uniform_location_id uniform, int v0, int v1)
{
    glUniform2i(uniform.get(), v0, v1);
}

void jkgm::gl::detail::inner_set_uniform_vec(uniform_location_id uniform, int v0, int v1, int v2)
{
    glUniform3i(uniform.get(), v0, v1, v2);
}

void jkgm::gl::detail::inner_set_uniform_vec(uniform_location_id uniform,
                                            int v0,
                                            int v1,
                                            int v2,
                                            int v3)
{
    glUniform4i(uniform.get(), v0, v1, v2, v3);
}

void jkgm::gl::detail::inner_set_uniform_vec(uniform_location_id uniform, float v0)
{
    glUniform1f(uniform.get(), v0);
}

void jkgm::gl::detail::inner_set_uniform_vec(uniform_location_id uniform, float v0, float v1)
{
    glUniform2f(uniform.get(), v0, v1);
}

void jkgm::gl::detail::inner_set_uniform_vec(uniform_location_id uniform,
                                            float v0,
                                            float v1,
                                            float v2)
{
    glUniform3f(uniform.get(), v0, v1, v2);
}

void jkgm::gl::detail::inner_set_uniform_vec(uniform_location_id uniform,
                                            float v0,
                                            float v1,
                                            float v2,
                                            float v3)
{
    glUniform4f(uniform.get(), v0, v1, v2, v3);
}
