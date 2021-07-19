#version 330 core

uniform sampler2D fbuf_image;

layout(location = 0) out vec4 out_color;

void main()
{
    ivec2 base_tc = ivec2(gl_FragCoord.xy) * 2;

    vec4 a = texelFetchOffset(fbuf_image, base_tc, /*lod*/ 0, ivec2(0, 0));
    vec4 b = texelFetchOffset(fbuf_image, base_tc, /*lod*/ 0, ivec2(1, 0));
    vec4 c = texelFetchOffset(fbuf_image, base_tc, /*lod*/ 0, ivec2(0, 1));
    vec4 d = texelFetchOffset(fbuf_image, base_tc, /*lod*/ 0, ivec2(1, 1));

    out_color = (a + b + c + d) * 0.25;
}