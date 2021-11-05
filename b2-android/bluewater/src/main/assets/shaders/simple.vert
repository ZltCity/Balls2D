#version 300 es

precision highp float;

layout(location = 0) in vec3 in_pos;
layout(location = 1) in vec4 in_color;

out vec4 frag_color;

uniform mat4 in_projection;
uniform mat4 in_modelview;

void main()
{
	vec4 position = in_projection * in_modelview * vec4(in_pos, 1.0);

	frag_color = in_color;
	gl_Position = position;
}
