#version 330

uniform sampler2D screen_texture;
uniform float time;
uniform float darken_screen_factor;
uniform vec2 resolution;
uniform float vision_radius;

in vec2 texcoord;

layout(location = 0) out vec4 color;

// this shader is inspired by: https://www.shadertoy.com/view/fdGfRV

// https://www.pcg-random.org/
uint pcg(uint v)
{
	uint state = v * 747796405u + 2891336453u;
	uint word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
	return (word >> 22u) ^ word;
}

uvec2 pcg2d(uvec2 v)
{
    v = v * 1664525u + 1013904223u;

    v.x += v.y * 1664525u;
    v.y += v.x * 1664525u;

    v = v ^ (v>>16u);

    v.x += v.y * 1664525u;
    v.y += v.x * 1664525u;

    v = v ^ (v>>16u);

    return v;
}

float HashToNoise(uint hash)
{
    return float(hash % 65536u) / 65536.0;
}

vec2 HashToNoise(uvec2 hash)
{
    return vec2(
        float(hash.x % 65536u) / 65536.0,
        float(hash.y % 65536u) / 65536.0
    );
}

mat2 RotationMatrix(float theta)
{
    float s = sin(theta);
    float c = cos(theta);
    return mat2(c, -s, s, c);
}

vec4 GetSnowLayer(vec2 uv, float layerScale, float layerIntensity) {
    uvec2 rc = uvec2(uv * vec2(50.0, 50.0) * layerScale);
    float colNoise0 = HashToNoise(pcg(rc.x));

    float snowFallDist = (colNoise0 * 2.0 + time * 5.0 * (layerScale * 0.9 + 0.1)) * 1.5;
    float heightScale = (colNoise0 * 3.0 + 5.0) * layerScale * 10.0;
    
    rc.y = uint(uv.y * heightScale + snowFallDist);
    float rowNoise0 = HashToNoise(pcg(rc.y));
    vec2 cellNoise0 = HashToNoise(pcg2d(rc));
    
    vec2 f = fract(uv * vec2(50.0, heightScale) + vec2(0.0, snowFallDist));
    f.y = 1.0 - f.y;

    float xOffset = (rowNoise0 * 2.0 - 1.0) * 0.4;
    
    vec2 sdf = vec2(abs((f.x + xOffset) * 2.0 - 1.0), f.y);
    
    float flakeVisibility = cellNoise0.x / 1.5;

    float v = max(0.0, 1.0 - sdf.x * 3.0) * f.y * flakeVisibility * layerIntensity;
    return vec4(vec3(1.0) * v, v);
}

vec4 snow()
{
    vec2 fragCoord = gl_FragCoord.xy;
    vec2 uv = fragCoord/resolution.xy;
    
    vec4 c = vec4(0.0);
    float layerThetaOffset = -0.20;
    
    for (uint i = 0u; i < 1u; ++i)
    {
        float layerNoise0 = HashToNoise(pcg(i));
        float layerTheta = (layerNoise0 * 2.0 - 1.0) * 0.011 + layerThetaOffset;
        vec2 layerUv = uv * RotationMatrix(layerTheta);

        float layerScale = sqrt(float(i) * 0.25 + 1.0);
        float layerIntensity = exp(-float(i + 1u) * 0.235 / 10.0);
        
        c += GetSnowLayer(layerUv + float(i) * vec2(223.32849, 432.3829), layerScale, layerIntensity);
    }

    vec4 icyHue = vec4(0.85, 0.95, 1.0, 1.0);
    vec4 snowColor = vec4(1.0);
    vec4 bg = mix(texture(screen_texture, texcoord), icyHue, 0.3);

    return mix(bg, snowColor, clamp(c.a, 0.0, 1.0));
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
    float t = time / 60.0;

    vec2 center = vec2(0.5, 0.5);
    vec2 aspect = vec2(resolution.x / resolution.y, 1.0);
    float dist = distance((texcoord - center) * aspect, vec2(0.0));

    float radius = vision_radius;

    float darkness = clamp(0.5 * (1.0 + tanh(k * cos(t - (3.0 * pi / 2.0)))), 0.0, 0.95);
    float light_strength = smoothstep(0, radius, dist);

    if (dist <= radius && darkness > 0.5) {
        return mix(in_color, vec4(0, 0, 0, 1), light_strength * darkness);
    } else {
        return mix(in_color, vec4(0, 0, 0, 1), darkness);
    }
}

void main()
{
    color = day_night_mix(vignette(snow()), 3.0);
}


