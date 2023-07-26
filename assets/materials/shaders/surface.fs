#version 300 es

precision highp float;

layout(location = 0) out vec4 out_color;

in vec3 normal;

void main()
{
	vec3 lightDir = vec3(0.0, -1.0, 0.0);

	out_color = normalize(vec4(0.0, 3.0, 7.0, 1.0)) * max(dot(normal, lightDir), 0.0);
}
