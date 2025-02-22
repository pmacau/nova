#version 330

uniform vec3 in_color;

// Output color
layout(location = 0) out vec4 color;

void main()
{
	color = vec4(in_color, 1.0);
}
