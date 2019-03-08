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

    color += texture(fbuf_image, vp_texcoords) * 0.1964825501511404;
    color += texture(fbuf_image, vp_texcoords + (off1 / fbuf_dimensions)) * 0.2969069646728344;
    color += texture(fbuf_image, vp_texcoords - (off1 / fbuf_dimensions)) * 0.2969069646728344;
    color += texture(fbuf_image, vp_texcoords + (off2 / fbuf_dimensions)) * 0.09447039785044732;
    color += texture(fbuf_image, vp_texcoords - (off2 / fbuf_dimensions)) * 0.09447039785044732;
    color += texture(fbuf_image, vp_texcoords + (off3 / fbuf_dimensions)) * 0.010381362401148057;
    color += texture(fbuf_image, vp_texcoords - (off3 / fbuf_dimensions)) * 0.010381362401148057;

    out_color = color;
}
