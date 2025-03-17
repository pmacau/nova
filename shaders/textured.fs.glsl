#version 330

// From vertex shader
in vec2 texcoord;

// Application data
uniform sampler2D sampler0;
uniform vec3 fcolor;

// Output color
layout(location = 0) out  vec4 color;

void main()
{
	vec4 texColor = texture(sampler0, texcoord);
	vec3 waterColor = vec3(65.0 / 255.0, 147.0 / 255.0, 226.0 / 255.0);
	vec3 tintColor = fcolor;

	// Don't recolor the water
	if (
		distance(texColor.rgb, waterColor) < 0.25 ||
		distance(tintColor, vec3(1, 1, 1)) < 0.1
	) {
		color = texColor;
	} else {
		color = mix(texColor, vec4(tintColor, 1.0), 0.3);
	}
}
