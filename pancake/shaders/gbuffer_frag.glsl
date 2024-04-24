#version 330 core
layout (location = 0) out vec4 albedo_out;
layout (location = 1) out vec4 normal_out;
layout (location = 2) out vec4 world_pos_out;

in vec3 world_pos;
in vec3 normal;
in vec2 tex_coords;

uniform vec4 colour;
uniform sampler2D tex;

void main()
{
    albedo_out = texture2D(tex, tex_coords).rgba * colour;
    normal_out = vec4(normal, 1);
    world_pos_out = vec4(world_pos, 1);
}