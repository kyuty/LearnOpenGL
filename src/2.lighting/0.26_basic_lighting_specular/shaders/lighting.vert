#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

out vec3 Normal; // 顶点法向量在世界坐标系下的法向量
out vec3 FragPos;// 顶点坐标在世界坐标系下的位置

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view *  model * vec4(position, 1.0f);
    FragPos = vec3(model * vec4(position, 1.0f));
    Normal = mat3(transpose(inverse(model))) * normal;  // 对于法线向量，我们只能对它应用缩放（scale）和旋转（rotation）变换
														// 
} 