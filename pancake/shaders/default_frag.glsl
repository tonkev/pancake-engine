#version 330 core
layout (location = 0) out vec4 frag_color;

in vec2 tex_coords;

uniform vec4 colour;
uniform sampler2D tex;

void main()
{
    frag_color = texture2D(tex, tex_coords).rgba * colour;
}