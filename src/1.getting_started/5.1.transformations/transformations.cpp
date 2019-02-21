#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/filesystem.h>
#include <learnopengl/shader_s.h>

#include <iostream>

//#define __MODE_ROTATE_TRANS__
//#define __MODE_TRANS_ROTATE_

//#define __MODE_ROTATE_SCALE__
//#define __MODE_SCALE_ROTATE__

//#define __MODE_TRANS_SCALE__
//#define __MODE_SCALE_TRANS__

//#define __MODE_TRANS_ROTATE_SCALE__
#define __MODE_SCALE_TRANS_ROTATE__

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
glm::mat4 method_rotate_trans(glm::vec3 trans, float degress, glm::vec3 axis);
glm::mat4 method_trans_rotate(glm::vec3 trans, float degress, glm::vec3 axis);
glm::mat4 method_rotate_scale(float degress, glm::vec3 axis, glm::vec3 scale);
glm::mat4 method_scale_rotate(float degress, glm::vec3 axis, glm::vec3 scale);
glm::mat4 method_scale_trans(glm::vec3 trans, glm::vec3 scale);
glm::mat4 method_trans_scale(glm::vec3 trans, glm::vec3 scale);
glm::mat4 method_trans_rotate_scale(glm::vec3 trans, float degress, glm::vec3 axis, glm::vec3 scale);
glm::mat4 method_scale_trans_rotate(glm::vec3 trans, float degress, glm::vec3 axis, glm::vec3 scale);
void printMat4(glm::mat4 transform);

// settings
const unsigned int SCR_WIDTH = 600;
const unsigned int SCR_HEIGHT = 600;

int main() {
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

    // glfw window creation
    // --------------------
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // build and compile our shader zprogram
    // ------------------------------------
    Shader ourShader("5.1.transform.vs", "5.1.transform.fs");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
            // positions          // texture coords
            0.5f, 0.5f, 0.0f, 1.0f, 1.0f, // top right
            0.5f, -0.5f, 0.0f, 1.0f, 0.0f, // bottom right
            -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // bottom left
            -0.5f, 0.5f, 0.0f, 0.0f, 1.0f  // top left
    };
    unsigned int indices[] = {
            0, 1, 3, // first triangle
            1, 2, 3  // second triangle
    };
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    // load and create a texture 
    // -------------------------
    unsigned int texture1, texture2;
    // texture 1
    // ---------
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    unsigned char *data = stbi_load(FileSystem::getPath("resources/textures/container.jpg").c_str(), &width, &height,
                                    &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    // texture 2
    // ---------
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    data = stbi_load(FileSystem::getPath("resources/textures/awesomeface.png").c_str(), &width, &height, &nrChannels,
                     0);
    if (data) {
        // note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    // -------------------------------------------------------------------------------------------
    ourShader.use();
    ourShader.setInt("texture1", 0);
    ourShader.setInt("texture2", 1);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // create transformations
    glm::mat4 transform(1.0f); // make sure to initialize matrix to identity matrix first

    {
        glm::vec3 trans(0.5f, -0.5f, 0.0f);
        float degress = 90.0f;
        glm::vec3 axis(0, 0, 1);
        glm::vec3 scale(0.5f, 0.5f, 0.5f);

        std::cout << "新的顶点 = 变换矩阵 * ori顶点" << std::endl;

#ifdef __MODE_ROTATE_TRANS__
        transform = method_rotate_trans(trans, degress, axis);
#endif

#ifdef __MODE_TRANS_ROTATE_
        transform = method_trans_rotate(trans, degress, axis);
#endif

#ifdef __MODE_ROTATE_SCALE__
        transform = method_rotate_scale(degress, axis, scale);
#endif

#ifdef __MODE_SCALE_ROTATE__
        transform = method_scale_rotate(degress, axis, scale);
#endif

#ifdef __MODE_TRANS_SCALE__
        transform = method_trans_scale(trans, scale);
#endif

#ifdef __MODE_SCALE_TRANS__
        transform = method_scale_trans(trans, scale);
#endif

#ifdef __MODE_TRANS_ROTATE_SCALE__
        transform = method_trans_rotate_scale(trans, degress, axis, scale);
#endif

#ifdef __MODE_SCALE_TRANS_ROTATE__
        transform = method_scale_trans_rotate(trans, degress, axis, scale);
#endif

        printMat4(transform);
    }

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // bind textures on corresponding texture units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        // get matrix's uniform location and set matrix
        ourShader.use();
        unsigned int transformLoc = glGetUniformLocation(ourShader.ID, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

        // render container
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

glm::mat4 method_rotate_trans(glm::vec3 trans, float degress, glm::vec3 axis) {
    std::cout << "变换矩阵 = 旋转*平移" << std::endl;
    glm::mat4 transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first

    transform = glm::rotate(transform, glm::radians(degress), axis); // Switched the order
    transform = glm::translate(transform, trans); // Switched the order

    std::cout << "对于ori顶点来说就是先平移后旋转" << std::endl;
    std::cout << "===============================" << std::endl;
    return transform;
}

glm::mat4 method_trans_rotate(glm::vec3 trans, float degress, glm::vec3 axis) {
    // Create transformations
    std::cout << "变换矩阵 = 平移*旋转" << std::endl;
    glm::mat4 transform1 = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first

    transform1 = glm::translate(transform1, trans);
    transform1 = glm::rotate(transform1, glm::radians(degress), axis);

    std::cout << "对于ori顶点来说就是先旋转后平移" << std::endl;
    std::cout << "===============================" << std::endl;
    return transform1;
}

glm::mat4 method_rotate_scale(float degress, glm::vec3 axis, glm::vec3 scale) {
    // Create transformations
    std::cout << "变换矩阵 = 旋转*缩放" << std::endl;
    glm::mat4 transform1 = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first

    transform1 = glm::rotate(transform1, glm::radians(degress), axis);
    transform1 = glm::scale(transform1, scale);

    std::cout << "对于ori顶点来说就是先旋转后缩放" << std::endl;
    std::cout << "===============================" << std::endl;
    return transform1;
}

glm::mat4 method_scale_rotate(float degress, glm::vec3 axis, glm::vec3 scale) {
    // Create transformations
    std::cout << "变换矩阵 = 缩放*旋转" << std::endl;
    glm::mat4 transform1 = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first

    transform1 = glm::scale(transform1, scale);
    transform1 = glm::rotate(transform1, glm::radians(degress), axis);

    std::cout << "对于ori顶点来说就是先旋转后缩放" << std::endl;
    std::cout << "===============================" << std::endl;
    return transform1;
}

glm::mat4 method_trans_scale(glm::vec3 trans, glm::vec3 scale) {
    // Create transformations
    std::cout << "变换矩阵 = 平移*缩放" << std::endl;
    glm::mat4 transform1 = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first

    transform1 = glm::translate(transform1, trans);
    transform1 = glm::scale(transform1, scale);

    std::cout << "对于ori顶点来说就是先缩放后平移" << std::endl;
    std::cout << "===============================" << std::endl;
    return transform1;
}

glm::mat4 method_scale_trans(glm::vec3 trans, glm::vec3 scale) {
    // Create transformations
    std::cout << "变换矩阵 = 缩放*平移" << std::endl;
    glm::mat4 transform1 = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first

    transform1 = glm::scale(transform1, scale);
    transform1 = glm::translate(transform1, trans);

    std::cout << "对于ori顶点来说就是先平移后缩放" << std::endl;
    std::cout << "===============================" << std::endl;
    return transform1;
}

glm::mat4 method_trans_rotate_scale(glm::vec3 trans, float degress, glm::vec3 axis, glm::vec3 scale) {
    // Create transformations
    std::cout << "变换矩阵 = 平移*旋转*缩放" << std::endl;
    glm::mat4 transform2 = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first

    transform2 = glm::translate(transform2, trans);
    transform2 = glm::rotate(transform2, glm::radians(degress), axis); // Switched the order
    transform2 = glm::scale(transform2, scale);

    std::cout << "对于ori顶点来说就是先缩放后旋转后平移" << std::endl;
    std::cout << "===============================" << std::endl;
    return transform2;
}

glm::mat4 method_scale_trans_rotate(glm::vec3 trans, float degress, glm::vec3 axis, glm::vec3 scale) {
    // Create transformations
    std::cout << "变换矩阵 = 缩放*平移*旋转" << std::endl;
    glm::mat4 transform2 = glm::mat4(1.0f);

    transform2 = glm::scale(transform2, scale);
    transform2 = glm::translate(transform2, trans);
    transform2 = glm::rotate(transform2, glm::radians(degress), axis); // Switched the order

    std::cout << "对于ori顶点来说就是先旋转后平移后缩放" << std::endl;
    std::cout << "===============================" << std::endl;

    return transform2;
}

void printMat4(glm::mat4 transform) {
    /*
    这是我打印的问题
    我打印矩阵的形式  看似是行矩阵
    那是因为我打印的摆放顺序不对，我应该列着摆放；但是我写的两个for是横着摆放的，所以看的像是列矩阵。
    实际上glm和OpenGL都是 列为主 的矩阵
    */
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            std::cout << transform[i][j] << "   ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}