#version 330

in vec3 color;

// Output color
layout(location = 0) out vec4 out_color;

void main()
{
	out_color = vec4(color, 0.0); // alpha of 0.0 for transparent
}
