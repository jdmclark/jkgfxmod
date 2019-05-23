#version 330 core
#extension GL_ARB_explicit_uniform_location : require

layout(location = 0) uniform sampler2D fbuf_image;
layout(location = 1) uniform sampler2D depth_image;

in vec2 vp_texcoords;

layout(location = 0) out vec4 out_color;

const ivec2[16] offsets = ivec2[16](
        ivec2(-2, -2),
        ivec2(-1, -2),
        ivec2( 0, -2),
        ivec2( 1, -2),
        ivec2(-2, -1),
        ivec2(-1, -1),
        ivec2( 0, -1),
        ivec2( 1, -1),
        ivec2(-2,  0),
        ivec2(-1,  0),
        ivec2( 0,  0),
        ivec2( 1,  0),
        ivec2(-2,  1),
        ivec2(-1,  1),
        ivec2( 0,  1),
        ivec2( 1,  1)
);

const float edge_depth_threshold = 0.1;
const float edge_normal_threshold = 0.9;

void main()
{
    float samps[16];
    float dsamps[16];
    vec3 nsamps[16];
    for(int i = 0; i < 16; ++i) {
        samps[i] = textureOffset(fbuf_image, vp_texcoords, offsets[i]).r;
        vec4 dnsamp = textureOffset(depth_image, vp_texcoords, offsets[i]);
        dsamps[i] = dnsamp.a;
        nsamps[i] = normalize(dnsamp.rgb);
    }

    float base_samp = samps[10];
    float base_dsamp = dsamps[10];
    vec3 base_nsamp = nsamps[10];

    float occlusion = 0.0;
    float occ_weights = 0.0;
    for(int i = 0; i < 16; ++i) {
        float weight = float(abs(dsamps[i] - base_dsamp) < edge_depth_threshold ||
                             dot(nsamps[i], base_nsamp) > edge_normal_threshold);
        occlusion += weight * samps[i];
        occ_weights += weight;
    }

    occlusion /= occ_weights;
    out_color = vec4(occlusion, 0.0, 0.0, 0.0);
}
