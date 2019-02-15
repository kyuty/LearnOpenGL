#version 330 core

in vec3 ourPosition;// in vec3 ourColor;

out vec4 color;

void main()
{
    color = vec4(ourPosition, 1.0f);    // Note how the position value is linearly interpolated to get all the different colors
										// 如果有小于0的值,插值到片元着色器的时候,小于0的值将会被设置为0.
										// (从另一个角度辅助思考:颜色值的rgb是没有负数的,所以小于0的都将会被认为0(黑色))
}
