#version 330 core
out vec4 FragColor;
  
uniform vec3 objectColor;
uniform vec3 lightColor;
in vec3 out_pos;

void main()
{
    vec3 final_color = vec3(objectColor.x, objectColor.y, objectColor.z);

/*
    float current_x = out_pos.x;
    float width = 1.0f;
    float minX = -0.5f;
    float split = width / 3.0f * 2 + minX;
    if (current_x > split) {
        final_color.x = mix(0, objectColor.x, 2 * current_x);
        final_color.y = mix(0, objectColor.y, 2 * current_x);
        final_color.z = mix(0, objectColor.z, 2 * current_x);
    }
*/

/*
    float current_x = out_pos.x;
    float current_y = out_pos.y;
    float minX = -0.5f;
    float minY = -0.5;
    float maxX = 0.5f;
    float maxY = 0.5f;
    float halfX = (minX + maxX) / 2;
    float halfY = (minY + maxY) / 2;
    // right top 0.5f, 0.5f     (x1, y1)
    // left bottom 0.0f, -0.5f  (x2, y2)
    // k = (y1 - y2) / (x1 - x2) // y = kx - 0.5f
    float k = (0.5f + 0.5f) / 0.5f;
    if (current_x >= halfX) {
        if (current_y <= (current_x * k - 0.5f)) {
            final_color.x = mix(0, objectColor.x, 2 * current_x);
            final_color.y = mix(0, objectColor.y, 2 * current_x);
            final_color.z = mix(0, objectColor.z, 2 * current_x);
        }
    }
*/

/*
    float current_x = out_pos.x;
    float current_y = out_pos.y;
    float radius = sqrt(pow(current_x, 2) + pow(current_y, 2));
    if (radius <= 0.5f) {
        final_color.x = mix(0, objectColor.x, 2 * radius);
        final_color.y = mix(0, objectColor.y, 2 * radius);
        final_color.z = mix(0, objectColor.z, 2 * radius);
    }
*/

    float current_x = out_pos.x;
    float current_y = out_pos.y;
    float radius = sqrt(pow(current_x, 2) + pow(current_y, 2));
    float sin_angle = current_y / radius;
    float cos_angle = current_x / radius;
    mat2 angle_mat2 = mat2(cos_angle, -sin_angle, sin_angle, cos_angle);
    vec2 final_color_2 = angle_mat2 * vec2(final_color.x, final_color.y);
    final_color.x = final_color_2.x;
    final_color.y = final_color_2.y;
    final_color.z = 1;

    FragColor = vec4(lightColor * final_color, 1.0);
}