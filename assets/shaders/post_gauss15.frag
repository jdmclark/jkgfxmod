#version 330 core

uniform sampler2D fbuf_image;
uniform ivec2 blur_direction;

layout(location = 0) out vec4 out_color;

vec4 fetch_sym(ivec2 tc, int radius, float fac)
{
    vec4 rv = texelFetch(fbuf_image, tc + radius * blur_direction, 0);
    rv += texelFetch(fbuf_image, tc - radius * blur_direction, 0);
    return rv * fac;
}

void main()
{
    ivec2 tc = ivec2(gl_FragCoord.xy);

    out_color = texelFetch(fbuf_image, tc + blur_direction, 0);
    out_color += texelFetch(fbuf_image, tc - blur_direction, 0);
    out_color *= 0.1478352381802946;

    out_color += texelFetch(fbuf_image, tc, 0) * 0.1601799366950943;

    out_color += fetch_sym(tc, 2, 0.11622102852187252);
    out_color += fetch_sym(tc, 3, 0.07782622199643041);
    out_color += fetch_sym(tc, 4, 0.04439114149471707);
    out_color += fetch_sym(tc, 5, 0.021566692402713306);
    out_color += fetch_sym(tc, 6, 0.008924355643617059);
    out_color += fetch_sym(tc, 7, 0.003145353412807818);
}
