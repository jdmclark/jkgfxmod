#version 330 core
#extension GL_ARB_explicit_uniform_location : require

layout(location = 0) uniform sampler2D fbuf_image;
layout(location = 1) uniform sampler2D normal_image;
layout(location = 2) uniform sampler2D depth_image;

in vec2 vp_texcoords;

layout(location = 0) out vec4 out_color;

const float edge_depth_threshold = 0.1;
const float edge_normal_threshold = 0.9;

void main()
{
    float samps[16];
    float dsamps[16];
    vec3 nsamps[16];

    samps[0]  = textureOffset(fbuf_image, vp_texcoords, ivec2(-2, -2)).r;
    samps[1]  = textureOffset(fbuf_image, vp_texcoords, ivec2(-1, -2)).r;
    samps[2]  = textureOffset(fbuf_image, vp_texcoords, ivec2( 0, -2)).r;
    samps[3]  = textureOffset(fbuf_image, vp_texcoords, ivec2( 1, -2)).r;
    samps[4]  = textureOffset(fbuf_image, vp_texcoords, ivec2(-2, -1)).r;
    samps[5]  = textureOffset(fbuf_image, vp_texcoords, ivec2(-1, -1)).r;
    samps[6]  = textureOffset(fbuf_image, vp_texcoords, ivec2( 0, -1)).r;
    samps[7]  = textureOffset(fbuf_image, vp_texcoords, ivec2( 1, -1)).r;
    samps[8]  = textureOffset(fbuf_image, vp_texcoords, ivec2(-2,  0)).r;
    samps[9]  = textureOffset(fbuf_image, vp_texcoords, ivec2(-1,  0)).r;
    samps[10] = textureOffset(fbuf_image, vp_texcoords, ivec2( 0,  0)).r;
    samps[11] = textureOffset(fbuf_image, vp_texcoords, ivec2( 1,  0)).r;
    samps[12] = textureOffset(fbuf_image, vp_texcoords, ivec2(-2,  1)).r;
    samps[13] = textureOffset(fbuf_image, vp_texcoords, ivec2(-1,  1)).r;
    samps[14] = textureOffset(fbuf_image, vp_texcoords, ivec2( 0,  1)).r;
    samps[15] = textureOffset(fbuf_image, vp_texcoords, ivec2( 1,  1)).r;

    dsamps[0]  = textureOffset(depth_image, vp_texcoords, ivec2(-2, -2)).r;
    dsamps[1]  = textureOffset(depth_image, vp_texcoords, ivec2(-1, -2)).r;
    dsamps[2]  = textureOffset(depth_image, vp_texcoords, ivec2( 0, -2)).r;
    dsamps[3]  = textureOffset(depth_image, vp_texcoords, ivec2( 1, -2)).r;
    dsamps[4]  = textureOffset(depth_image, vp_texcoords, ivec2(-2, -1)).r;
    dsamps[5]  = textureOffset(depth_image, vp_texcoords, ivec2(-1, -1)).r;
    dsamps[6]  = textureOffset(depth_image, vp_texcoords, ivec2( 0, -1)).r;
    dsamps[7]  = textureOffset(depth_image, vp_texcoords, ivec2( 1, -1)).r;
    dsamps[8]  = textureOffset(depth_image, vp_texcoords, ivec2(-2,  0)).r;
    dsamps[9]  = textureOffset(depth_image, vp_texcoords, ivec2(-1,  0)).r;
    dsamps[10] = textureOffset(depth_image, vp_texcoords, ivec2( 0,  0)).r;
    dsamps[11] = textureOffset(depth_image, vp_texcoords, ivec2( 1,  0)).r;
    dsamps[12] = textureOffset(depth_image, vp_texcoords, ivec2(-2,  1)).r;
    dsamps[13] = textureOffset(depth_image, vp_texcoords, ivec2(-1,  1)).r;
    dsamps[14] = textureOffset(depth_image, vp_texcoords, ivec2( 0,  1)).r;
    dsamps[15] = textureOffset(depth_image, vp_texcoords, ivec2( 1,  1)).r;

    nsamps[0]  = normalize(textureOffset(depth_image, vp_texcoords, ivec2(-2, -2)).rgb);
    nsamps[1]  = normalize(textureOffset(depth_image, vp_texcoords, ivec2(-1, -2)).rgb);
    nsamps[2]  = normalize(textureOffset(depth_image, vp_texcoords, ivec2( 0, -2)).rgb);
    nsamps[3]  = normalize(textureOffset(depth_image, vp_texcoords, ivec2( 1, -2)).rgb);
    nsamps[4]  = normalize(textureOffset(depth_image, vp_texcoords, ivec2(-2, -1)).rgb);
    nsamps[5]  = normalize(textureOffset(depth_image, vp_texcoords, ivec2(-1, -1)).rgb);
    nsamps[6]  = normalize(textureOffset(depth_image, vp_texcoords, ivec2( 0, -1)).rgb);
    nsamps[7]  = normalize(textureOffset(depth_image, vp_texcoords, ivec2( 1, -1)).rgb);
    nsamps[8]  = normalize(textureOffset(depth_image, vp_texcoords, ivec2(-2,  0)).rgb);
    nsamps[9]  = normalize(textureOffset(depth_image, vp_texcoords, ivec2(-1,  0)).rgb);
    nsamps[10] = normalize(textureOffset(depth_image, vp_texcoords, ivec2( 0,  0)).rgb);
    nsamps[11] = normalize(textureOffset(depth_image, vp_texcoords, ivec2( 1,  0)).rgb);
    nsamps[12] = normalize(textureOffset(depth_image, vp_texcoords, ivec2(-2,  1)).rgb);
    nsamps[13] = normalize(textureOffset(depth_image, vp_texcoords, ivec2(-1,  1)).rgb);
    nsamps[14] = normalize(textureOffset(depth_image, vp_texcoords, ivec2( 0,  1)).rgb);
    nsamps[15] = normalize(textureOffset(depth_image, vp_texcoords, ivec2( 1,  1)).rgb);

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
