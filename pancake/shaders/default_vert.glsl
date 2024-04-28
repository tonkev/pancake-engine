#version 330 core
layout(location = 0) in vec4 m_pos;
layout(location = 1) in vec4 m_normal;
layout(location = 4) in vec2 m_tex_coords;
layout(location = 6) in mat4 mvp;

out vec2 tex_coords;

uniform vec4 tex_transform;

void main()
{
    gl_Position = mvp * vec4(m_pos.xyz, 1.0);
    tex_coords = (m_tex_coords * tex_transform.zw) + tex_transform.xy;
}