介绍：

绘制10个立方体，可以用键盘wasd控制移动，用鼠标移动控制look方向，用鼠标滚轮控制视角。

![result](../7.3.camera_mouse_zoom/result.gif)



利用camera的封装类。

```
// wasd键盘 上下左右
camera.ProcessKeyboard(FORWARD, deltaTime);
camera.ProcessKeyboard(BACKWARD, deltaTime);
camera.ProcessKeyboard(LEFT, deltaTime);
camera.ProcessKeyboard(RIGHT, deltaTime);

// 移动鼠标，改变look方向
camera.ProcessMouseMovement(xoffset, yoffset);

// 滚轮改变fov
camera.ProcessMouseScroll(yoffset);

// Default camera values
// camera的静态成员变量，默认值如下
const float YAW         = -90.0f;
const float PITCH       =  0.0f;
const float SPEED       =  2.5f;
const float SENSITIVITY =  0.1f;
const float ZOOM        =  45.0f;
```

