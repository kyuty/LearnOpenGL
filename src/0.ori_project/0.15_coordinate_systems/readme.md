物体绕x轴正方向，逆时针转了55度

然后z轴移动-3距离。



simple code

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

