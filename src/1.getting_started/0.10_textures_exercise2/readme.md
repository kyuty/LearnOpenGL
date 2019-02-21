介绍：

在箱子的四个角落放置4个笑脸。

![result](/Users/wangdong/github/opengl/LearnOpenGL/src/1.getting_started/0.10_textures_exercise2/result.png)

---

纹理坐标从0-1改变成0-2. 对GL_REPEAT有更深刻的理解.

```
GLfloat vertices[] = {
		// Positions          // Colors           // Texture Coords  (Note that we changed them to 2.0f!)
		0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   2.0f, 2.0f, // Top Right
		0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   2.0f, 0.0f, // Bottom Right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // Bottom Left
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 2.0f  // Top Left 
	};
```

