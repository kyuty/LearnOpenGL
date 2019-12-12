#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

// texture samplers
uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
	// linearly interpolate between both textures (80% container, 20% awesomeface)
	FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2) * vec4(ourColor, 1);

	//FragColor.r = 1;
	//FragColor.g = 1;
	//FragColor.b = 1;
	//FragColor.a = 0; // 如果没开blend或者只开了blend没有设置blend模式，那么a是不生效（不清楚只开blend的时候，gl的blend模式状态是什么）
	                   // 只有既开了blend，并且设置了blend模式，这是a才生效
	                   // glEnable(GL_BLEND);                                // 开blend
                       // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // 设置blend模式
}