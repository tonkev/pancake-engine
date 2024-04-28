#version 330 core
layout(location = 0) in vec4 m_pos;
layout(location = 1) in vec4 m_normal;
layout(location = 4) in vec2 m_tex_coords;
layout(location = 6) in mat4 mvp;

out vec3 world_pos;
out vec3 normal;
out vec2 tex_coords;

uniform vec4 tex_transform;

void main()
{
    vec4 wpos = mvp * vec4(m_pos.xyz, 1.0);
    gl_Position = wpos;
    world_pos = wpos.xyz;
    normal = m_normal.xyz;
    tex_coords = (m_tex_coords * tex_transform.zw) + tex_transform.xy;
}