物体绕x轴正方向，逆时针转了55度

然后z轴移动-3距离。

![image-20190221200038311](/Users/wangdong/github/opengl/LearnOpenGL/src/1.getting_started/6.1.coordinate_systems/result.jpg)



###simple code

model：model矩阵进行了旋转操作

view：view矩阵进行了平移操作

projection：投影矩阵

```
glm::mat4 model         = glm::mat4(1.0f); 
glm::mat4 view          = glm::mat4(1.0f);
glm::mat4 projection    = glm::mat4(1.0f);

model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
view  = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

unsigned int modelLoc = glGetUniformLocation(ourShader.ID, "model");
unsigned int viewLoc  = glGetUniformLocation(ourShader.ID, "view");
// 三种方式给shader传值
glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
ourShader.setMat4("projection", projection);
```

注意mvp乘的顺序

```
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}
```



###Perspective和Frustum参数互转

```
void perspectiveToFrustum() {
    // 已知 fov aspect near far (fov指的是bottom top方向的弧度)
    // 求 left right bottom top

    // 已知 --------------- begin
    float zNear = 0.1f;
    float aspect = 1.0f; // (float) SCR_WIDTH / (float) SCR_HEIGHT
    float zFar = 100.0f;
    float dbFov = 45.0f * M_PI / 180.0;
    // 已知 --------------- end
    // 打印已知的projection
    glm::mat4 projectionKnow = glm::perspective(dbFov, aspect, zNear, zFar);
    printMat4(projectionKnow);


    // 计算frustum参数
    float dbTan = tan(dbFov / 2);
    float resultLeftRight = zNear * dbTan * aspect * 2;
    float resultBottomTop = zNear * dbTan * 2;
    std::cout << "resultLeftRight = " << resultLeftRight << std::endl;
    std::cout << "resultBottomTop = " << resultBottomTop << std::endl;
    glm::mat4 projection = glm::frustum(-resultLeftRight / 2, resultLeftRight / 2,
                              -resultBottomTop / 2, resultBottomTop / 2, zNear, zFar);
    printMat4(projection);
}

void frustumToPerspective() {
    // 已知 left right bottom top near far
    // 求 fov aspect (fov指的是bottom top方向的弧度)

    // 已知 --------------- begin
    float width = 0.0828427f;
    float height = 0.0828427f;
    float left = -width / 2;
    float right = width / 2;
    float top = height / 2;
    float bottom = -height / 2;
    float near = 0.1f;
    float far = 100.0f;
    // 已知 --------------- end
    // 打印已知的projection
    glm::mat4 projectionKnow = glm::frustum(left, right, bottom, top, near, far);
    printMat4(projectionKnow);


    // 计算perspective参数
    float resultAspect = width / height;
    float resultFov = atan2(top, near) * 2;
    std::cout << "resultAspect = " << resultAspect << std::endl;
    std::cout << "resultFov = " << (resultFov / M_PI * 180) << std::endl;

    glm::mat4 projection = glm::perspective(resultFov, resultAspect, near, far);
    printMat4(projection);
}

打印结果：最后算出来的projection矩阵相同
2.41421   0   0   0   
0   2.41421   0   0   
0   0   -1.002   -1   
0   0   -0.2002   0   

resultLeftRight = 0.0828427
resultBottomTop = 0.0828427
2.41421   0   0   0   
0   2.41421   0   0   
0   0   -1.002   -1   
0   0   -0.2002   0   

2.41421   0   0   0   
0   2.41421   0   0   
0   0   -1.002   -1   
0   0   -0.2002   0   

resultAspect = 1
resultFov = 45
2.41421   0   0   0   
0   2.41421   0   0   
0   0   -1.002   -1   
0   0   -0.2002   0   
```



通过perspective和frustum互转，可以算出，本demo的

`projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);`

投影矩阵，计算完之后，内容区域的左右宽度为0.0828427，上下高度为0.0828427。且该投影矩阵的位置在(0, 0, 0)，近平面是z轴为-1.0f的平面。（**近平面是z轴为-1，x范围在[-0.0828427 / 2.0f, 0.0828427 / 2.0]，y范围在[-0.0828427 / 2.0f, 0.0828427 / 2.0f]**）

