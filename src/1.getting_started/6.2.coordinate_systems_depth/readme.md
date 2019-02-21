介绍：

画一个立方体，并不停的旋转。

**需要开启深度检测，否则绘制会出错**。

### Simple code

```
glEnable(GL_DEPTH_TEST);

...

while (!glfwWindowShouldClose(window))
{
	...
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!
	
	...
}
```



开启深度检测：

![result_open_depth](/Users/wangdong/github/opengl/LearnOpenGL/src/1.getting_started/6.2.coordinate_systems_depth/result_open_depth.gif)

不开深度检测：

```
//glEnable(GL_DEPTH_TEST);

...

while (!glfwWindowShouldClose(window))
{
	...
	
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT);
	
	...
}
```



![result_close_depth](/Users/wangdong/github/opengl/LearnOpenGL/src/1.getting_started/6.2.coordinate_systems_depth/result_close_depth.gif)