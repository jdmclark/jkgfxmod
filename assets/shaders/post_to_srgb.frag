#version 330 core
#extension GL_ARB_explicit_uniform_location : require

layout(location = 0) uniform sampler2D fbuf_image;

const int num_bloom_layers = 4;
layout(location = 1) uniform sampler2D bloom_fbuf[num_bloom_layers];
layout(location = 5) uniform float bloom_weight[num_bloom_layers];

in vec2 vp_texcoords;

layout(location = 0) out vec4 out_color;

void main()
{
    ivec2 sc = ivec2(gl_FragCoord.xy);
    vec4 samp = texelFetch(fbuf_image, sc, 0);

    vec4 bloom_samp = vec4(0.0);
    float bloom_sum = 0.0;

    for(int i = 0; i < num_bloom_layers; ++i) {
        bloom_samp += texture(bloom_fbuf[i], vp_texcoords) * bloom_weight[i];
        bloom_sum += bloom_weight[i];
    }

    vec3 combined_color = samp.rgb + (bloom_samp.rgb / (bloom_sum));

    out_color = vec4(pow(combined_color, vec3(1.0 / 2.2)), samp.a);
}
