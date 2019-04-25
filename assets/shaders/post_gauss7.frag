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

    vec2 off1 = vec2(1.411764705882353) * blur_direction;
    vec2 off2 = vec2(3.2941176470588234) * blur_direction;
    vec2 off3 = vec2(5.176470588235294) * blur_direction;

    color += texture(fbuf_image, vp_texcoords) * 0.3300522707216076;
    color += texture(fbuf_image, vp_texcoords + (off1 / fbuf_dimensions)) * 0.2184169438598873;
    color += texture(fbuf_image, vp_texcoords - (off1 / fbuf_dimensions)) * 0.2184169438598873;
    color += texture(fbuf_image, vp_texcoords + (off2 / fbuf_dimensions)) * 0.1050166315932388;
    color += texture(fbuf_image, vp_texcoords - (off2 / fbuf_dimensions)) * 0.1050166315932388;
    color += texture(fbuf_image, vp_texcoords + (off3 / fbuf_dimensions)) * 0.0115402891860701;
    color += texture(fbuf_image, vp_texcoords - (off3 / fbuf_dimensions)) * 0.0115402891860701;

    out_color = color;
}
