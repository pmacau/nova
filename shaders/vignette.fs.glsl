#version 330

uniform sampler2D screen_texture;
uniform float time;
uniform float darken_screen_factor;

in vec2 texcoord;

layout(location = 0) out vec4 color;


// M1 interpolation implementation
vec4 vignette(vec4 in_color) 
{
	vec2 center = vec2(0.5, 0.5);
    float dist = distance(texcoord, center) * 1.75;
    float vignetteFactor = smoothstep(0.5, 1.2, dist);

    vec3 redTint = vec3(1.0, 0.0, 0.0);

    vec3 finalColor = mix(in_color.rgb, redTint, darken_screen_factor * vignetteFactor);
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