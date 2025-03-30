#version 330

uniform sampler2D screen_texture;
uniform float time;
uniform float darken_screen_factor;
uniform vec2 resolution;

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

vec4 day_night_mix(vec4 in_color, float k) {
    float pi = 3.1415926;
    float t = 4 * time / (60.0 * pi);

    vec2 center = vec2(0.5, 0.5);
    vec2 aspect = vec2(resolution.x / resolution.y, 1.0);
    float dist = distance((texcoord - center) * aspect, vec2(0.0));

    float radius = 0.1;

    float darkness = clamp(0.5 * (1.0 + tanh(k * sin(t - (5.0 * pi / 6.0)))), 0.0, 0.95);
    float light_strength = smoothstep(0, radius, dist);

    if (dist <= radius && darkness > 0.5) {
        return mix(in_color, vec4(0, 0, 0, 1), light_strength * darkness);
    } else {
        return mix(in_color, vec4(0, 0, 0, 1), darkness);
    }
}

void main()
{
    vec4 in_color = texture(screen_texture, texcoord);
    color = day_night_mix(vignette(in_color), 3.0);
}