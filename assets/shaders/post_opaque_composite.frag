#version 330 core
#extension GL_ARB_explicit_uniform_location : require

layout(location = 0) uniform sampler2D color_image;
layout(location = 1) uniform sampler2D emissive_image;
layout(location = 2) uniform sampler2D occlusion_image;

layout(location = 0) out vec4 out_color;

void main()
{
    ivec2 tc = ivec2(gl_FragCoord.xy);

    vec4 albedo = texelFetch(color_image, tc, 0);
    vec4 emissive = texelFetch(emissive_image, tc, 0);
    float occlusion = 1.0 - texelFetch(occlusion_image, tc, 0).r;

    out_color = vec4(emissive.rgb + (albedo.rgb * occlusion), albedo.a);
}