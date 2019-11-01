#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 Position;

uniform vec3 cameraPos;
uniform samplerCube skybox;

void main()
{    
    vec3 I = normalize(Position - cameraPos); // I向量方向为 摄像机方向 指向 物体位置；单位化。为入射光向量。
    vec3 R = reflect(I, normalize(Normal));   // R向量 = I向量 与 单位化后的法向量Normal，做reflect反射
    FragColor = vec4(texture(skybox, R).rgb, 1.0); // R向量直接作用于天空盒贴图采样（此位置的贴图使用 R向量 去 采样 samplerCube纹理）
                                                   // 这里 texture函数中：一个samplerCube 是 需要与 一个vec3的纹理坐标 计算的
}