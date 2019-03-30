#version 330 core
#extension GL_ARB_explicit_uniform_location : require

layout(location = 0) in vec2 vertex_position;
layout(location = 1) in vec2 vertex_texcoords;

out vec2 vp_texcoords;

void main()
{
    gl_Position = vec4(vertex_position, 0.0, 1.0);
    vp_texcoords = vertex_texcoords;
}
