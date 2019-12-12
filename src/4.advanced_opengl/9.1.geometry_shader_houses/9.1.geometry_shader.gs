#version 330 core
layout (points) in; // 指定输入的点是什么类型，point表示输入的一个点
                    // 所以最下面gl_in数组长度是1
                    // 如果是lines，则gl_in长度应该是2
layout (triangle_strip, max_vertices = 5) out; // 指定几何着色器输出的图元类型

in VS_OUT {
    vec3 color;
} gsIn[];

out vec3 fColor;

// 有多少个顶点，这里就走多少次，vs走4次，则gs也走4次。
// 这里会单独处理每一个顶点，这里是将1个点变成5个点
void build_house(vec4 position)
{   
    fColor = gsIn[0].color; // gsIn[0] since there's only one input vertex
    gl_Position = position + vec4(-0.2, -0.2, 0.0, 0.0); // 1:bottom-left
    EmitVertex();           // 创建左下的点，然后提交，此时提交顶点对应的颜色是fColor
    gl_Position = position + vec4( 0.2, -0.2, 0.0, 0.0); // 2:bottom-right
    EmitVertex();
    gl_Position = position + vec4(-0.2,  0.2, 0.0, 0.0); // 3:top-left
    EmitVertex();
    gl_Position = position + vec4( 0.2,  0.2, 0.0, 0.0); // 4:top-right
    EmitVertex();
    gl_Position = position + vec4( 0.0,  0.4, 0.0, 0.0); // 5:top
    fColor = vec3(1.0, 1.0, 1.0); // 这个颜色值只生效于top点
    EmitVertex();                 // 创建top的点，然后提交，此时提交顶点对应的颜色是fColor
    EndPrimitive();               // 最后调用 EndPrimitive 结束
}

void main() {
    // 通过 gl_in[0].gl_Position 可以获取从vs传来的顶点数据
    // gl_in是内建变量，它在shader内部可能是这样的：
    //   in gl_Vertex
    //   {
    //       vec4  gl_Position;
    //      float gl_PointSize;
    //       float gl_ClipDistance[];
    //   } gl_in[];
    build_house(gl_in[0].gl_Position);
}