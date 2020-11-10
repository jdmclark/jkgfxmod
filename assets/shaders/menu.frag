#version 330 core

uniform sampler2D tex;

in vec2 vp_texcoords;

layout(location = 0) out vec4 out_color;

void main()
{
    out_color = texture(tex, vp_texcoords);
}
