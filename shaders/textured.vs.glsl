#version 330

// Input attributes
in vec3 in_position;
in vec2 in_texcoord;

// Passed to fragment shader
out vec2 texcoord;

// Application data
uniform mat3 transform;
uniform mat3 projection;

uniform vec2 u_SpriteSheetSize;
uniform vec2 u_SpriteSheetSelection;

void main()
{
	vec2 spriteSize = vec2(1.0) / u_SpriteSheetSize;

	texcoord = (u_SpriteSheetSelection + in_texcoord) * spriteSize;

	vec3 pos = projection * transform * vec3(in_position.xy, 1.0);
	gl_Position = vec4(pos.xy, in_position.z, 1.0);
}