#version 300 es

precision highp float;

layout(location = 0) out vec4 out_color;

in float point_size;
in vec2 particle_center;

void main()
{
	float distance = length(gl_FragCoord.xy - particle_center), factor = distance / (point_size * 0.5);

	if (factor > 1.0)
		discard;

	factor = 1.0 - factor;

	out_color = vec4(factor, factor, factor, factor); //vec4(normalize(vec3(gl_FragCoord.xy - particle_center, -1.0)), factor);
}
