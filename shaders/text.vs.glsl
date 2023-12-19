#version 330 core
layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
out vec2 TexCoords;

uniform mat3 projection;

void main()
{
    vec3 pos = projection * vec3(vertex.xy, 1.0);
    gl_Position = vec4(pos.xy, 0.5, 1.0);
    TexCoords = vertex.zw;
}