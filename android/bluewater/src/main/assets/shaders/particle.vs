#version 300 es

precision highp float;

layout(location = 0) in vec3 in_pos;

uniform float in_point_size;
uniform vec2 in_surface_size;
uniform mat4 in_projection;
uniform mat4 in_modelview;

out float point_size;
out vec2 particle_center;

vec2 projectPoint(vec4 position);
float getPointSize(vec4 position, float radius);

void main()
{
	vec4 position = in_projection * in_modelview * vec4(in_pos, 1.0);

	point_size = in_point_size * getPointSize(position, 0.5);
	gl_PointSize = point_size;
	gl_Position = position;
	particle_center = projectPoint(position);
}

vec2 projectPoint(vec4 position)
{
	return vec2(
		(position.x / position.w + 1.0) * 0.5 * in_surface_size.x,
		(position.y / position.w + 1.0) * 0.5 * in_surface_size.y);
}

float getPointSize(vec4 position, float radius)
{
	vec3 right = vec3(in_modelview[0][0], in_modelview[1][0], in_modelview[2][0]);
	vec2 p1 = vec2(projectPoint(position)),
		p2 = vec2(
			projectPoint(in_projection * in_modelview * vec4(in_pos + right * radius, 1.0)));

	return abs(p1.x - p2.x);
}
