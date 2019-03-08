#version 330 core
#extension GL_ARB_explicit_uniform_location : require

layout(location = 0) uniform sampler2D tex;

in vec2 vp_texcoords;

layout(location = 0) out vec4 out_color;

void main()
{
    out_color = texture(tex, vp_texcoords);
}
