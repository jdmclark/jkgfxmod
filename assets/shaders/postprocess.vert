#version 330 core

layout(location = 0) in vec2 vertex_position;

out vec2 vp_texcoords;

void main()
{
    gl_Position = vec4(vertex_position, 0.0, 1.0);
    vp_texcoords = (vertex_position + 1.0) / 2.0;
}
