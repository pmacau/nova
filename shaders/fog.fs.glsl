#version 330

uniform sampler2D screen_texture;
uniform float time;
uniform float darken_screen_factor;
uniform vec2 resolution;

in vec2 texcoord;

layout(location = 0) out vec4 color;

float random(vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9818,79.279))) * 43758.5453123);
}

vec2 random2(vec2 st) {
    st = vec2(dot(st, vec2(127.1, 311.7)),
              dot(st, vec2(269.5, 183.3)));
    return -1.0 + 2.0 * fract(sin(st) * 7.0);
}

float noise(vec2 st) {
    vec2 i = floor(st);
    vec2 f = fract(st);
    vec2 u = f * f * (3.0 - 2.0 * f);
    return mix(
        mix(dot(random2(i + vec2(0.0, 0.0)), f - vec2(0.0, 0.0)),
            dot(random2(i + vec2(1.0, 0.0)), f - vec2(1.0, 0.0)), u.x),
        mix(dot(random2(i + vec2(0.0, 1.0)), f - vec2(0.0, 1.0)),
            dot(random2(i + vec2(1.0, 1.0)), f - vec2(1.0, 1.0)), u.x),
        u.y);
}

float fractal_brownian_motion(vec2 coord) {
    float value = 0.0;
    float scale = 0.2;
    for (int i = 0; i < 4; i++) {
        value += noise(coord) * scale;
        coord *= 2.0;
        scale *= 0.5;
    }
    return value + 0.2;
}

vec4 fog() {
    vec2 fragCoord = gl_FragCoord.xy;
    vec2 st = fragCoord / resolution.xy;
    st *= resolution.xy / resolution.y;
    vec2 pos = st * 3.0;
    
    // Increase animation speed by doubling the time multipliers.
    vec2 motion = vec2(fractal_brownian_motion(pos + vec2(time * -1.0, time * -0.6)));
    
    // Increase fog visibility by boosting the mixing value.
    float final = fractal_brownian_motion(pos + motion) * 1.5;
    final = clamp(final, 0.0, 1.0);
    
    //vec3 fogColor = vec3(0.42, 0.40, 0.47);
    vec3 fogColor = vec3(0.71, 0.35, 1.0);
    vec3 in_color = texture(screen_texture, texcoord).rgb;
    return vec4(mix(in_color, fogColor, final), 1.0);
}

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
    color = day_night_mix(vignette(fog()), 3.0);
}