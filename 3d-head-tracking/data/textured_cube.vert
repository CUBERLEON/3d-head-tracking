#version 330 core

layout(location = 0) in vec3 v_pos;
layout(location = 1) in vec2 v_uv;

out vec2 f_uv;

uniform mat4 v_viewProj;
uniform mat4 v_model;

uniform float v_eyes_x;
uniform float v_eyes_y;
uniform float v_z_offset;

vec3 transformPos(vec3 pos, float alpha_x, float alpha_y, float z_offset)
{
    pos.x += 2.f * (pos.z - z_offset) * sin(alpha_x);
    pos.y += 2.f * (pos.z - z_offset) * sin(alpha_y);
    return pos;
}

void main()
{
    vec3 worldPos = transformPos((v_model * vec4(v_pos, 1)).xyz, v_eyes_x, v_eyes_y, v_z_offset);
	
    gl_Position =  v_viewProj * vec4(worldPos, 1);
	f_uv = v_uv.xy;
}