#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 Normal;
out vec3 Position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    Normal = mat3(transpose(inverse(model))) * aNormal; // 这里提供 顶点着色器 中时时计算法向量的一种方法：model矩阵先逆矩阵，再转置，再取3x3，然后乘以 法向量
    Position = vec3(model * vec4(aPos, 1.0)); // model矩阵 直接乘以 位置坐标，就是该坐标在物理世界上的位置，为方便在 片源着色器 里 计算入射光向量 用的
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}