#version 330 core

layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
out vec2 TexCoords;

uniform mat3 projection;

void main() {
    gl_Position = vec4(projection * vec3(vertex.xy, 1.0), 1.0);
    TexCoords = vertex.zw;
}