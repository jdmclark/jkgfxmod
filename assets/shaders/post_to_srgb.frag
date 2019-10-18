#version 330 core
#extension GL_ARB_explicit_uniform_location : require

layout(location = 0) uniform sampler2D fbuf_image;

const int num_bloom_layers = 4;
layout(location = 1) uniform sampler2D bloom_fbuf[num_bloom_layers];
layout(location = 5) uniform float bloom_weight[num_bloom_layers];

in vec2 vp_texcoords;

layout(location = 0) out vec4 out_color;

vec3 color_to_srgb(vec3 c)
{
    bvec3 cutoff = greaterThan(c, vec3(0.0031308));
    vec3 linear_version = 12.92 * c;
    vec3 exp_version = 1.055 * pow(c, vec3(1.0 / 2.4)) - vec3(0.055);

    return mix(linear_version, exp_version, cutoff);
}

void main()
{
    ivec2 sc = ivec2(gl_FragCoord.xy);
    vec4 samp = texelFetch(fbuf_image, sc, 0);

    vec4 bloom_samp = vec4(0.0);
    float bloom_sum = 0.0;

    bloom_samp += texture(bloom_fbuf[0], vp_texcoords) * bloom_weight[0];
    bloom_sum += bloom_weight[0];
    bloom_samp += texture(bloom_fbuf[1], vp_texcoords) * bloom_weight[1];
    bloom_sum += bloom_weight[1];
    bloom_samp += texture(bloom_fbuf[2], vp_texcoords) * bloom_weight[2];
    bloom_sum += bloom_weight[2];
    bloom_samp += texture(bloom_fbuf[3], vp_texcoords) * bloom_weight[3];
    bloom_sum += bloom_weight[3];

    vec3 combined_color = samp.rgb + (bloom_samp.rgb / (bloom_sum));
    out_color = vec4(color_to_srgb(combined_color), samp.a);
}
