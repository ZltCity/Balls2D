#version 300 es

precision highp float;

layout(location = 0) in vec3 in_pos;
layout(location = 1) in vec3 in_normal;

uniform mat4 in_projection;
uniform mat4 in_modelview;

out vec3 normal;

void main()
{
	gl_Position = in_projection * in_modelview * vec4(in_pos, 1.0);
	normal = in_normal;
}
