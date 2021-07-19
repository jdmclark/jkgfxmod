#version 330 core

uniform sampler2D fbuf_image;

const int num_bloom_layers = 5;
uniform sampler2D bloom_fbuf[num_bloom_layers];
uniform float bloom_weight[num_bloom_layers];

in vec2 vp_texcoords;

layout(location = 0) out vec4 out_color;

void main()
{
    ivec2 sc = ivec2(gl_FragCoord.xy);
    vec4 samp = texelFetch(fbuf_image, sc, 0);

    vec4 bloom_samp = vec4(0.0);
    bloom_samp += texture(bloom_fbuf[0], vp_texcoords) * bloom_weight[0];
    bloom_samp += texture(bloom_fbuf[1], vp_texcoords) * bloom_weight[1];
    bloom_samp += texture(bloom_fbuf[2], vp_texcoords) * bloom_weight[2];
    bloom_samp += texture(bloom_fbuf[3], vp_texcoords) * bloom_weight[3];
    bloom_samp += texture(bloom_fbuf[4], vp_texcoords) * bloom_weight[4];

    vec3 combined_color = samp.rgb + bloom_samp.rgb;
    out_color = vec4(combined_color, samp.a);
}