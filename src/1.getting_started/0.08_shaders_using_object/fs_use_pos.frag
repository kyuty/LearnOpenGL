#version 330 core

in vec3 ourPosition;// in vec3 ourColor;

out vec4 color;

void main()
{
    color = vec4(ourPosition, 1.0f);    // Note how the position value is linearly interpolated to get all the different colors
										// 如果有小于0的值,插值到片元着色器的时候,小于0的值将会被设置为0.
										// (从另一个角度辅助思考:颜色值的rgb是没有负数的,所以小于0的都将会被认为0(黑色))
}
/*
Answer to the question: Do you know why the bottom-left side is black?
为什么左下角是黑色的
-- --------------------------------------------------------------------
Think about this for a second: the output of our fragment's color is equal to the (interpolated) coordinate of
the triangle. What is the coordinate of the bottom-left point of our triangle? This is (-0.5f, -0.5f, 0.0f). Since the
xy values are negative they are clamped to a value of 0.0f. This happens all the way to the center sides of the
triangle since from that point on the values will be interpolated positively again. Values of 0.0f are of course black
and that explains the black side of the triangle.

想一想这一点：我们片段颜色的输出等于（插值）坐标三角形。 三角形左下角的坐标是多少？ 这是（-0.5f，-0.5f，0.0f）。
自从xy值为负，它们被强制置为0.0f的值。 这种情况一直发生在中心两侧从这一点开始，三角形将再次积极插值。 0.0f的值当然是黑色的这解释了三角形的黑色面。
*/