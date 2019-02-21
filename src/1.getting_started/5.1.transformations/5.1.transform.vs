#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 transform;

void main()
{
	gl_Position = transform * vec4(aPos, 1.0); // vec4解释为4x1矩阵，并非1x4矩阵
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}