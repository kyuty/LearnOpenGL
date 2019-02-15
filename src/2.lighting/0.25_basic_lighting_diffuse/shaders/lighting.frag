#version 330 core
out vec4 color;
  
in vec3 Normal;  
in vec3 FragPos;  
  
uniform vec3 lightPos; // 在游戏循环中更新uniform;如果其值永远不会改变,也可以在游戏循环外设置一次uniform的值.
uniform vec3 lightColor;
uniform vec3 objectColor;

void main()
{
    // Ambient
    float ambientStrength = 0.1f;
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 result = (ambient + diffuse) * objectColor;
    color = vec4(result, 1.0f);
} 