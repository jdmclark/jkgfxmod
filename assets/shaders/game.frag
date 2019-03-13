#version 330 core
#extension GL_ARB_explicit_uniform_location : require

layout(location = 1) uniform sampler2D albedo_map;
layout(location = 2) uniform vec4 albedo_factor;

layout(location = 3) uniform sampler2D emissive_map;
layout(location = 4) uniform vec3 emissive_factor;

in vec2 vp_texcoords;
in vec4 vp_color;

layout(location = 0) out vec4 out_color;

void main()
{
    vec4 tex_color = texture(albedo_map, vp_texcoords);
    vec4 albedo = tex_color * vp_color * albedo_factor;

    vec3 emissive = texture(emissive_map, vp_texcoords).rgb * emissive_factor;

    out_color = vec4(emissive + albedo.rgb, albedo.a);
}