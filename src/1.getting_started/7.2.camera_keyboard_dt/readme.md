介绍：

绘制10个立方体，可以通过键盘的wasd键移动摄像机。

该摄像机永远 look的点 是 摄像机当前位置往z轴负方向1个单位。

![result](/Users/wangdong/github/opengl/LearnOpenGL/src/1.getting_started/7.2.camera_keyboard_dt/result.gif)



### Simple code

glm::cross(cameraFront, cameraUp) ：叉乘

w键逻辑：当前位置cameraPos + look方向向量 * 速度。 速度 = delta时间 * 系数2.5

s键逻辑：当前位置cameraPos - look方向向量 * 速度。 速度 = delta时间 * 系数2.5

a键逻辑：1.计算look方向向量与摄像机up向量的叉乘；2.将叉乘单位化；3.乘以速度；4.当前位置减去上述结果

d键逻辑：1.计算look方向向量与摄像机up向量的叉乘；2.将叉乘单位化；3.乘以速度；4.当前位置加上上述结果

```
// camera
glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f,  3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

while (!glfwWindowShouldClose(window))
{
    // per-frame time logic
    // --------------------
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    
    // input
    processInput(window);
    
    ......
    
    // camera/view transformation
    // 该摄像机永远 look的点 是 摄像机当前位置往z轴负方向1个单位。
    glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    ourShader.setMat4("view", view);
    
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = 2.5 * deltaTime; 
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}
```

