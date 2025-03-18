#version 330

uniform sampler2D screen_texture;
uniform vec2 resolution;
uniform float time;

in vec2 texcoord;

layout(location = 0) out vec4 color;

#define TILES 5.0

float random(vec2 uv) {
    return fract(sin(dot(uv, vec2(135.0, 263.0))) * 103.214532);
}

vec4 drawSnow(vec2 curid, vec2 uv, vec4 fragColor, float r, float c) {
    float maxoff = 2.0 / TILES;

    for (int x = -2; x <= 1; x++) {
        for (int y = -2; y <= 0; y++) {
            float rad = (1.0 / (TILES * 5.0)) * r;
            vec2 id = curid + vec2(x, y);
            vec2 pos = id / TILES;

            float xmod = mod(random(pos), maxoff);
            pos.x += xmod;
            pos.y += mod(random(pos + vec2(4, 3)), maxoff);
            rad *= mod(random(pos), 1.0);
            pos.x += 0.5 * (maxoff - xmod) * sin(time * r + random(pos) * 100.0);

            float len = length(uv - pos);
            float v = smoothstep(0.0, 1.0, (rad - len) / rad * 0.75);
            fragColor = mix(fragColor, vec4(c), v);
        }
    }

    return fragColor;
}

out vec4 FragColor;

void main() {
    vec2 fragCoord = gl_FragCoord.xy;
    vec2 uvNorm = fragCoord / resolution;
    vec2 uvog = fragCoord / resolution.y;
    vec2 uv = fragCoord / resolution.y;

    // Sample the frame buffer as the base background
    vec4 bg = texture(screen_texture, uvNorm);

    // Draw closest snow layer
    uv += 0.2 * vec2(-time, time);
    vec2 curid = floor(uv * TILES) + vec2(0.5);
    vec4 snowLayer = drawSnow(curid, uv, bg, 1.0, 0.9);

    // Draw middle snow layer
    // uv = uvog + 0.1 * vec2(-time - 100.0, time + 100.0);
    // curid = floor(uv * TILES) + vec2(0.5);
    // snowLayer += drawSnow(curid, uv, vec4(0), 0.75, 0.45);

    // Draw far snow layer
    uv = uvog + 0.05 * vec2(-time - 150.0, time + 150.0);
    curid = floor(uv * TILES) + vec2(0.5);
    snowLayer += drawSnow(curid, uv, vec4(0), 0.5, 0.225);

    // Blend the snow with the original frame buffer
    FragColor = mix(mix(bg, snowLayer, 0.5), vec4(1, 1, 1, 1), 0.3);
}
