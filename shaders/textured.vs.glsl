#version 330

// Input attributes
in vec3 in_position;
in vec2 in_texcoord;

// Passed to fragment shader
out vec2 texcoord;

// Application data
uniform mat3 transform;
uniform mat3 projection;

uniform mat3 model_transform;
uniform mat3 camera_transform;

uniform vec4 spriteData; // row, col, spriteW, spriteH
uniform vec2 sheetDims; // width, height

uniform vec3 cameraPos;

uniform float zValue;

vec2 center_texcoord(vec2 in_tex) {
	float row = spriteData.x;
	float col = spriteData.y;
	float spriteW = spriteData.z;
	float spriteH = spriteData.w;

	return vec2(
		(in_tex.x + col) * spriteW / sheetDims.x,
		(in_tex.y + row) * spriteH / sheetDims.y
	);
}

// (0, 0) -> bottom left
// (x * spriteW + (col * spriteW))

void main()
{
	texcoord = center_texcoord(in_texcoord);

	vec3 pos2D_world = model_transform * vec3(in_position.xy, 1.0);
	vec3 pos2D_camera = camera_transform * vec3(pos2D_world.xy, 1.0);
	vec3 pos2D_clip = projection * vec3(pos2D_camera.xy, 1.0);

	float camera_v_dist = distance(cameraPos, vec3(pos2D_world.xy, 0.0));

	float z = camera_v_dist / 10000.0;


	// vec3 pos = projection * transform * vec3(in_position.xy, 1.0);
	// gl_Position = vec4(pos.xy, in_position.z, 1.0);
	gl_Position = vec4(pos2D_clip.xy, zValue, 1.0);
}