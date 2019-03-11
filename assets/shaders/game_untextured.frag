#version 330 core
#extension GL_ARB_explicit_uniform_location : require

in vec2 vp_texcoords;
in vec4 vp_color;

layout(location = 0) out vec4 out_color;

void main()
{
    vec4 albedo = vp_color;

    out_color = albedo;
}