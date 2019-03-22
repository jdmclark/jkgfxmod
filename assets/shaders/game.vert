#version 330 core
#extension GL_ARB_explicit_uniform_location : require

layout(location = 0) in vec4 vertex_position;
layout(location = 1) in vec2 vertex_texcoords;
layout(location = 2) in vec4 vertex_color;
layout(location = 3) in vec3 vertex_normal;

layout(location = 0) uniform vec2 screen_resolution;

out vec2 vp_texcoords;
out vec4 vp_color;
out vec3 vp_normal;
out float vp_z;

void main()
{
    gl_Position = vertex_position;

    vp_texcoords = vertex_texcoords;
    vp_color = vertex_color;
    vp_normal = vertex_normal;
    vp_z = vertex_position.w;
}
