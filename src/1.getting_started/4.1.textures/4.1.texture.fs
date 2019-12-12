#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

// texture sampler
uniform sampler2D texture1;

void main()
{
/*
        vec2 tex_coord = vec2(TexCoord.x - 0.5f, TexCoord.y - 0.5f);
        float current_x = tex_coord.x;
        float current_y = tex_coord.y;
        float radius = sqrt(pow(current_x, 2) + pow(current_y, 2));
        float sin_angle = current_y / radius;
        float cos_angle = current_x / radius;
        mat2 angle_mat2 = mat2(cos_angle, -sin_angle, sin_angle, cos_angle);
        tex_coord = angle_mat2 * vec2(tex_coord.x, tex_coord.y);

        FragColor = texture(texture1, tex_coord);
*/

	FragColor = texture(texture1, TexCoord);
}