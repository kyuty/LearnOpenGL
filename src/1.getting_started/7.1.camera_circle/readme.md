介绍：

绘制10个立方体，摄像机围绕着中心点不停转动。

![result](/Users/wangdong/github/opengl/LearnOpenGL/src/1.getting_started/7.1.camera_circle/result.gif)



### Simple code

这里camera，实际上就是view矩阵。通常camera会与projection乘在一起。

使用glm::lookAt函数。

```
glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
ourShader.setMat4("projection", projection); 

while (!glfwWindowShouldClose(window))
{
    // camera/view transformation
    glm::mat4 view = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    float radius = 10.0f;
    float camX   = sin(glfwGetTime()) * radius;
    float camZ   = cos(glfwGetTime()) * radius;
    view = glm::lookAt(glm::vec3(camX, 0.0f, camZ), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    ourShader.setMat4("view", view);

    // render boxes
    glBindVertexArray(VAO);
    for (unsigned int i = 0; i < 10; i++)
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, cubePositions[i]);
        float angle = 20.0f * i;
        model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
        ourShader.setMat4("model", model);

    	glDrawArrays(GL_TRIANGLES, 0, 36);
   }
}
```

