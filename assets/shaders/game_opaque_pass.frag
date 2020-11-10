#version 330 core

uniform vec4 features;

uniform sampler2D albedo_map;
uniform vec4 albedo_factor;

uniform sampler2D emissive_map;
uniform vec3 emissive_factor;

uniform float alpha_cutoff;

uniform sampler2D displacement_map;
uniform float displacement_factor;

in vec3 vp_pos;
in vec2 vp_texcoords;
in vec4 vp_color;
in vec3 vp_normal;
in float vp_z;

layout(location = 0) out vec4 out_color;
layout(location = 1) out vec4 out_emissive;
layout(location = 2) out vec4 out_normal;
layout(location = 3) out vec4 out_depth;

mat3 construct_tbn()
{
    vec3 n = normalize(vp_normal);

    vec3 dp1 = dFdx(vp_pos);
    vec3 dp2 = dFdy(vp_pos);
    vec2 duv1 = dFdx(vp_texcoords);
    vec2 duv2 = dFdy(vp_texcoords);

    vec3 dp2perp = cross(dp2, n);
    vec3 dp1perp = cross(n, dp1);

    vec3 t = dp2perp * duv1.x + dp1perp * duv2.x;
    vec3 b = dp2perp * duv1.y + dp1perp * duv2.y;

    float invmax = inversesqrt(max(dot(t, t), dot(b, b)));
    return mat3(t * invmax, b * invmax, n);
}

vec3 parallax_mapping(vec2 tc)
{
    // The injector world space view position is always considered (0, 0, 0):
    vec3 view_dir = -normalize(transpose(construct_tbn()) * vp_pos);

    const float min_layers = 32.0;
    const float max_layers = 128.0;
    float num_layers = mix(max_layers, min_layers, abs(dot(vec3(0.0, 0.0, 1.0), view_dir)));

    float layer_depth = 1.0 / num_layers;
    float current_layer_depth = 0.0;
    vec2 shift_per_layer = (view_dir.xy / view_dir.z) * displacement_factor;
    vec2 d_tc = shift_per_layer / num_layers;

    vec2 current_tc = tc;
    float current_sample = texture(displacement_map, current_tc).r;

    while(current_layer_depth < current_sample) {
        current_tc -= d_tc;
        current_sample = texture(displacement_map, current_tc).r;
        current_layer_depth += layer_depth;
    }

    vec2 prev_tc = current_tc + d_tc;

    float after_col_depth = current_sample - current_layer_depth;
    float before_col_depth = texture(displacement_map, prev_tc).r - current_layer_depth + layer_depth;

    float a = after_col_depth / (after_col_depth - before_col_depth);
    vec2 adj_tc = mix(current_tc, prev_tc, a);

    // Calculate depth bias for SSAO
    vec3 tbn_fragdelta = vec3(adj_tc - tc, mix(after_col_depth, before_col_depth, a));
    float adj_vp_z = length(tbn_fragdelta);

    return vec3(adj_tc, adj_vp_z);
}

void main()
{
    vec2 adj_texcoords = vp_texcoords;
    float adj_vp_z = vp_z;

    if(displacement_factor != 0.0) {
        vec3 pmapv = parallax_mapping(vp_texcoords);
        adj_texcoords = pmapv.xy;
        adj_vp_z += pmapv.z;
    }

    vec4 albedo_map_sample = texture(albedo_map, adj_texcoords);
    albedo_map_sample = mix(vec4(1.0), albedo_map_sample, features.x);

    if(features.z > 0.5) {
        if(albedo_map_sample.a < alpha_cutoff) {
            discard;
        }

        albedo_map_sample.a = 1.0;
    }

    vec4 posterized_vertex_color = vec4(ceil(vp_color * 64.0)) / 64.0;
    vec4 vertex_color = mix(vp_color, posterized_vertex_color, features.w);

    vec4 albedo = albedo_map_sample * vertex_color * albedo_factor;

    if(albedo.a < 0.99999f) {
        discard;
    }

    vec3 emissive_map_sample = texture(emissive_map, adj_texcoords).rgb;
    emissive_map_sample = mix(vec3(1.0), emissive_map_sample, features.y);
    vec3 emissive = emissive_map_sample * emissive_factor;

    out_color = albedo;
    out_emissive = vec4(emissive, albedo.a);
    out_normal = vec4(normalize(vp_normal), 1.0);
    out_depth = vec4(adj_vp_z, 0.0, 0.0, 1.0);
}