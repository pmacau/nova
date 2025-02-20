#version 330

// !!! Simple shader for colouring basic meshes

// Input attributes
in vec3 in_position;

// Application data
uniform mat3 transform;
uniform mat3 projection;

void main()
{
	vec3 pos = projection * transform * vec3(in_position.xy, 1.0);
	gl_Position = vec4(pos.xy, 0.0, 1.0);
	gl_PointSize = 10.0;
}
