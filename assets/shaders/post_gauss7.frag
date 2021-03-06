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
    out_color *= 0.235836;

    out_color += texelFetch(fbuf_image, tc, 0) * 0.33441;

    out_color += fetch_sym(tc, 2, 0.082624);
    out_color += fetch_sym(tc, 3, 0.014335);
}