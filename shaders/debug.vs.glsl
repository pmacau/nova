layout (location = 0) in vec2 position;
uniform mat3 transform;
uniform mat3 projection;

void main() {
    vec3 pos = projection * transform * vec3(position, 1.0);
    gl_Position = vec4(pos.xy, 0.0, 1.0);
}