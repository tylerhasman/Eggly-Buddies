#version 330

// Input attributes
layout(location = 0) in vec3 in_position;
layout(location = 1) in vec2 in_texcoord;
layout(location = 2) in vec2 shifted_position;
// layout(location = 3) in vec2 shifted_texcoord;
layout(location = 4) in float in_time;
layout(location = 5) in float in_scale;

// Passed to fragment shader
out vec2 texcoord;

// Application data
uniform mat3 transform;
uniform mat3 projection;
uniform float u_Time;

void main()
{
	texcoord = in_texcoord;

	float shrinkGrow = abs(sin(u_Time + in_time));
	vec2 scaledPosition = in_position.xy * in_scale * shrinkGrow;
	vec3 pos = projection * transform * vec3(scaledPosition + shifted_position, 1.0);
	gl_Position = vec4(pos.xy, in_position.z, 1.0);
}