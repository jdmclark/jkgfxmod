#version 330 core
#extension GL_ARB_explicit_uniform_location : require

layout(location = 0) in vec4 vertex_position;
layout(location = 8) in vec2 vertex_texcoords;
layout(location = 3) in vec4 vertex_color;
layout(location = 4) in vec3 vertex_specular;

out vec2 vp_texcoords;
out vec4 vp_color;
out vec3 vp_specular;

void main()
{
    // Convert vertex position from 640x480 viewport into OpenGL device coordinates
    float w = 1.0 / vertex_position.w;
    gl_Position = vec4(w * ((vertex_position.x / 320.0) - 1.0),
                       w * ((-vertex_position.y / 240.0) + 1.0),
                       w * (-vertex_position.z),
                       w);
    vp_texcoords = vertex_texcoords;
    vp_color = vertex_color;
    vp_specular = vertex_specular;
}
