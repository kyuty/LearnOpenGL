#version 330 core
in vec2 TexCoords;

out vec4 color;

uniform sampler2D texture1;

float LinearizeDepth(float depth);

void main()
{             
	//1.正常模式
    //color = texture(texture1, TexCoords);
	//2.观看深度值的变化:这是非线性的.物体离摄像机近的情况下,物体会发黑,因为z值接近0;稍微远些,物体就会马上变白,因为z值接近1.
	//color = vec4(vec3(gl_FragCoord.z),1.0f);

	//3.线性的深度变化:物体离摄像机越远,物体越白.
	float depth = LinearizeDepth(gl_FragCoord.z);
    color = vec4(vec3(depth), 1.0f);
}

float LinearizeDepth(float depth)
{
    float near = 0.1; 
    float far = 100.0; 
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * near) / (far + near - z * (far - near)); 
}