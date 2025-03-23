#version 330 core
layout (location = 0) in vec2 position; // Input vertex position

uniform mat3 projection; // Projection matrix to transform UI coordinates

void main() {
    vec3 pos = projection * vec3(position, 1.0); // Transform position
    gl_Position = vec4(pos.xy, 0.0, 1.0);
}