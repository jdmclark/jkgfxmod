#version 330 core
#extension GL_ARB_explicit_uniform_location : require

layout(location = 0) uniform sampler2D fbuf_image;
layout(location = 1) uniform vec2 fbuf_dimensions;
layout(location = 2) uniform vec2 blur_direction;

in vec2 vp_texcoords;

layout(location = 0) out vec4 out_color;

void main()
{
    vec4 color = vec4(0.0);

    vec2 off1 = vec2(1.3333333333333333) * blur_direction;

    color += texture(fbuf_image, vp_texcoords) * 0.29411764705882354;
    color += texture(fbuf_image, vp_texcoords + (off1 / fbuf_dimensions)) * 0.35294117647058826;
    color += texture(fbuf_image, vp_texcoords - (off1 / fbuf_dimensions)) * 0.35294117647058826;

    out_color = color;
}
