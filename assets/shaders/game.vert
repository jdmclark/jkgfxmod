#version 330 core
#extension GL_ARB_explicit_uniform_location : require

layout(location = 0) in vec4 vertex_position;
layout(location = 8) in vec2 vertex_texcoords;
layout(location = 3) in vec4 vertex_color;

layout(location = 0) uniform vec2 screen_resolution;

out vec2 vp_texcoords;
out vec4 vp_color;

void main()
{
    // Convert vertex position from 640x480 viewport into OpenGL device coordinates
    float w = 1.0 / vertex_position.w;
    gl_Position = vec4(w * ((vertex_position.x / (screen_resolution.x * 0.5)) - 1.0),
                       w * ((-vertex_position.y / (screen_resolution.y * 0.5)) + 1.0),
                       w * (-vertex_position.z),
                       w);
    vp_texcoords = vertex_texcoords;
    vp_color = vertex_color;
}
