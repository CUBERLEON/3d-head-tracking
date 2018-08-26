#version 330 core

in vec2 f_uv;

out vec3 color;

uniform sampler2D diffuseMap;

void main()
{
	color = texture(diffuseMap, f_uv).xyz;
    //color = vec3(f_uv.x, f_uv.y, 0);
    //color = vec3(1, 0, 0);
}