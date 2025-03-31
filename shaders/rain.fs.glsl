#version 330

uniform sampler2D screen_texture;
uniform float time;
uniform float darken_screen_factor;
uniform vec2 resolution;

in vec2 texcoord;

layout(location = 0) out vec4 color;

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

vec4 GetRainLayer(vec2 uv, float layerScale, float layerIntensity, float layerBlueShift)
{
    uvec2 rc = uvec2(uv * vec2(50.0, 10.0) * layerScale);
    float colNoise0 = HashToNoise(pcg(rc.x));
    
    // Random speed and drop length
    float rainFallDist = (colNoise0 * 2.0 + time * 5.0 * (layerScale * 0.9 + 0.1));
    float heightScale = (colNoise0 * 3.0 + 5.0) * layerScale;
    
    rc.y = uint(uv.y * heightScale + rainFallDist);
    float rowNoise0 = HashToNoise(pcg(rc.y));
    vec2 cellNoise0 = HashToNoise(pcg2d(rc));
    
    // Local coords within cell
    vec2 f = fract(uv * vec2(50.0, heightScale) + vec2(0.0, rainFallDist));
    f.y = 1.0 - f.y;
    
    // Shift drop left or right
    float xOffset = (rowNoise0 * 2.0 - 1.0) * 0.4;
    
    vec2 sdf = vec2(abs((f.x + xOffset) * 2.0 - 1.0), f.y);
    
    float dropVisibility = cellNoise0.x;
    float v = max(0.0, 1.0 - sdf.x * 3.0) * f.y * dropVisibility * layerIntensity;
    
    return vec4(mix(vec3(1.0), vec3(0.2, 0.5, 1.0), layerBlueShift) * v, v);
}

vec4 rain()
{
    vec2 fragCoord = gl_FragCoord.xy;
    // Normalized pixel coordinates (from 0 to 1)
    vec2 uv = fragCoord/resolution.xy;
    
    // Divide screen into different views
    uint viewType = 0u;
    
    vec4 c = vec4(0.0);
    float layerThetaOffset = -0.08; // Global rotation for all layers
    
    for (uint i = 0u; i < 1u; ++i)
    {
        // Setup layer properties
        float layerNoise0 = HashToNoise(pcg(i));
        float layerTheta = (layerNoise0 * 2.0 - 1.0) * 0.011 + layerThetaOffset; // Random theta per layer
        vec2 layerUv = uv * RotationMatrix(layerTheta);
        
        // Farther raindrops should be smaller, less intense, and bluer
        float layerScale = sqrt(float(i) * 0.25 + 1.0);
        float layerIntensity = exp(-float(i + 1u) * 0.235);
        float layerBlueShift = 1.0 - exp(-float(i + 1u) * 0.25);
        
        c += (1.0 - c.a) * GetRainLayer(layerUv + float(i) * vec2(223.32849, 432.3829), layerScale, layerIntensity, layerBlueShift);
    }

    vec4 bg = texture(screen_texture, texcoord);
    return mix(bg, vec4(c.rgb, 1.0), 0.3);
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
    color = day_night_mix(vignette(rain()), 3.0);
}