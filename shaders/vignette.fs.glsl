#version 330

uniform sampler2D screen_texture;
uniform float time;
uniform float darken_screen_factor;

in vec2 texcoord;

layout(location = 0) out vec4 color;

float hermite_interp(float a, float b, float x) {
    float scaled_x = clamp((x - a) / (b - a), 0.0, 1.0);
    return scaled_x * scaled_x * (3.0 - 2.0 * scaled_x);
}

vec3 color_lerp(vec3 c1, vec3 c2, float t) {
    return c1 * (1 - t) + c2 * t;
}

// M1 interpolation implementation
vec4 vignette(vec4 in_color) 
{
	vec2 center = vec2(0.5, 0.5);
    float dist = distance(texcoord, center) * 1.75;
    float vignetteFactor = hermite_interp(0.5, 1.2, dist);

    vec3 redTint = vec3(1.0, 0.0, 0.0);

    vec3 finalColor = color_lerp(in_color.rgb, redTint, darken_screen_factor * vignetteFactor);
	in_color = vec4(finalColor, 1);

	return in_color;
}

// TODO: uncomment this is we want to implement this again in the future
// darken the screen, i.e., fade to black
// vec4 fade_color(vec4 in_color) 
// {
// 	if (darken_screen_factor > 0)
// 		in_color -= darken_screen_factor * vec4(0.8, 0.8, 0.8, 0);
// 	return in_color;
// }

void main()
{
    vec4 in_color = texture(screen_texture, texcoord);
    color = vignette(in_color);
}