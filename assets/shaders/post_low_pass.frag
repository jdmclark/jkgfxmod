#version 330 core
#extension GL_ARB_explicit_uniform_location : require

layout(location = 0) uniform sampler2D fbuf_image;

in vec2 vp_texcoords;

layout(location = 0) out vec4 out_color;

void main()
{
    vec4 samp = texture(fbuf_image, vp_texcoords);

    vec3 col = samp.rgb;

    float lum = dot(vec3(0.2125, 0.7154, 0.0721), col);
    lum = max(0.0, lum * 0.5);

    float x = lum * lum * lum;
    float fac = smoothstep(0.0, 1.0, x);

    out_color = vec4(col * fac, samp.a);
}
