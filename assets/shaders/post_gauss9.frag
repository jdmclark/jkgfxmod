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
    out_color *= 0.22248396662566527;

    out_color += texelFetch(fbuf_image, tc, 0) * 0.289141483254238;

    out_color += fetch_sym(tc, 2, 0.10132378056964125);
    out_color += fetch_sym(tc, 3, 0.02728409576222039);
    out_color += fetch_sym(tc, 4, 0.004337415415354041);
}