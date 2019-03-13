#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/filesystem.h>
#include <learnopengl/shader_m.h>

#include <iostream>

/*
 * 相同depth 相同layer
 * 1.先画A，再画B // 现象：A先画的，A先把自己深度信息写在深度缓冲里，B后面画的时候，由于OpenGL默认深度检测的Function为GL_LESS（小于才通过），则A和B重叠部分还是显示A
 * 2.先画B，后画A // 现象：B先画的，B先把自己深度信息写在深度缓冲里，A后面画的时候，由于OpenGL默认深度检测的Function为GL_LESS（小于才通过），则A和B重叠部分还是显示B
 *
 * 结论：在相同layer和相同depth的情况下，覆盖区域是A还是B跟绘制顺序有关。
 * 不管先画谁，都有可能带来闪烁问题
 */
//#define __MODE_SAME_DEPTH_SAME_LAYER_A_B__ // depth_A: 0, depth_B: 0; layer_A: 0, layer_B: 0; first draw A, second draw B
//#define __MODE_SAME_DEPTH_SAME_LAYER_B_A__ // depth_A: 0, depth_B: 0; layer_A: 0, layer_B: 0; first draw B, second draw A

/*
 * 相同depth A的layer大，B的layer小
 * 1.先画A，再画B（先画layer比较大的，再画layer比较小的）// 现象：A盖着B；在深度相同的情况下，以layer为准，跟绘制顺序无关，但这种情况可能会影响blend出错
 * 2.先画B，后画A（先画layer比较小的，再画layer比较大的; 这是正确的绘制操作）// 现象：A盖着B；在深度相同的情况下，以layer为准，跟绘制顺序无关
 *
 * 结论：在相同depth的情况下，覆盖区域是A还是B跟layer有关，谁大谁在上，跟绘制顺序无关
 */
//#define __MODE_SAME_DEPTH_DIFF_LAYER_A_B__ // depth_A: 0, depth_B: 0; layer_A: 1, layer_B: 0; first draw A, second draw B
//#define __MODE_SAME_DEPTH_DIFF_LAYER_B_A__ // depth_A: 0, depth_B: 0; layer_A: 1, layer_B: 0; first draw B, second draw A

/*
 * 相同layer A的深度比较深，B的深度比较浅（A是远处物体，B是近处物体）
 * 1.先画A，再画B（先画远处物体，再画近处物体；这是正确的绘制操作）// 现象：B盖着A
 * 2.先画B，后画A（先画近处物体，再画远处物体）// 现象：B盖着A；虽然是先画的近处物体B，但由于深度检测是打开的，所以后面画A的时候，深度检测不通过，A画不上覆盖区域，所以还是B盖着A（这种情况会导致blend不对）
 *
 * 结论：在相同layer的情况下，覆盖区域是A还是B跟depth有关，谁浅谁在上，跟绘制顺序无关
 */
//#define __MODE_DIFF_DEPTH_SAME_LAYER_A_B__ // depth_A: 0, depth_B: 1; layer_A: 0, layer_B: 0; first draw A, second draw B
//#define __MODE_DIFF_DEPTH_SAME_LAYER_B_A__ // depth_A: 0, depth_B: 1; layer_A: 0, layer_B: 0; first draw B, second draw A

/*
 * A的深度比较深，B的深度比较浅（A是远处物体，B是近处物体）
 * 1.先画A，再画B（先画远处物体，再画近处物体；远处物体的layer小，近处物体的layer大；这是正常的绘制操作）// 现象：A盖着B
 * 2.先画A，再画B（先画远处物体，再画近处物体；远处物体的layer大，近处物体的layer小；这是不正常的绘制操作，深度绘制顺序是正确的，layer顺序是错误的，到底是以谁为生效呢？）// 现象：B盖着A。深度检测的优先级要大于layer，B在近处，所以B盖着A。
 * 3.先画B，后画A（先画近处物体，再画远处物体；远处物体的layer大，近处物体的layer小；这是不正常的绘制操作，深度绘制顺序是错误的，layer顺序是正确的，到底是以谁为生效呢？）// 现象：B盖着A。深度检测的优先级要大于layer，B在近处，所以B盖着A。跟绘制顺序无关。
 * 4.先画B，后画A（先画近处物体，再画远处物体；远处物体的layer小，近处物体的layer大；这是不正常的绘制操作，深度绘制顺序是错误的，layer顺序是错误的，到底是以谁为生效呢？）// 现象：B盖着A。深度检测的优先级要大于layer，B在近处，所以B盖着A。跟layer和绘制顺序无关。
 *
 * 结论：深度检测的优先级要 大于 layer层级的比较，跟绘制顺序也没有任何关系，绘制顺序只是影响blend可能会出问题。
 */
//#define __MODE_DIFF_DEPTH_DIFF_LAYER_A_B_1__ // depth_A: 0, depth_B: 1; layer_A: 1, layer_B: 2; first draw A, second draw B
//#define __MODE_DIFF_DEPTH_DIFF_LAYER_A_B_2__ // depth_A: 0, depth_B: 1; layer_A: 2, layer_B: 1; first draw A, second draw B
//#define __MODE_DIFF_DEPTH_DIFF_LAYER_B_A_1__ // depth_A: 0, depth_B: 1; layer_A: 2, layer_B: 1; first draw B, second draw A
#define __MODE_DIFF_DEPTH_DIFF_LAYER_B_A_2__ // depth_A: 0, depth_B: 1; layer_A: 1, layer_B: 2; first draw B, second draw A

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void draw(unsigned int VAO, unsigned int modelLoc, int layer, glm::mat4 model);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main()
{
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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader zprogram
    // ------------------------------------
    Shader ourShader("01.coordinate_systems.vs", "01.coordinate_systems.fs");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        -0.5f, -0.5f, -0.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.0f,  1.0f, 1.0f,

         0.5f,  0.5f, -0.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.0f,  0.0f, 0.0f,
    };
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    // load and create a texture 
    // -------------------------
    unsigned int texture1;
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
    unsigned char *data = stbi_load(FileSystem::getPath("resources/textures/container.jpg").c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    // -------------------------------------------------------------------------------------------
    ourShader.use();
    ourShader.setInt("texture1", 0);

    glEnable(GL_POLYGON_OFFSET_FILL);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!

        // bind textures on corresponding texture units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);

        // activate shader
        ourShader.use();

        // create transformations
        glm::mat4 view          = glm::mat4(1.0f);
        glm::mat4 projection    = glm::mat4(1.0f);
        view  = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
        projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        // retrieve the matrix uniform locations
        unsigned int modelLoc = glGetUniformLocation(ourShader.ID, "model");
        unsigned int viewLoc  = glGetUniformLocation(ourShader.ID, "view");
        // pass them to the shaders (3 different ways)
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
        // note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
        ourShader.setMat4("projection", projection);

        glm::mat4 model;
        int layerA, layerB;

#ifdef __MODE_SAME_DEPTH_SAME_LAYER_A_B__
        // render A box
        layerA = 0;
        model = glm::mat4(1.0f);
        draw(VAO, modelLoc, layerA, model);

        // render B box
        layerB = 0;
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.5f, 0.3f, 0));
        draw(VAO, modelLoc, layerB, model);
#endif

#ifdef __MODE_SAME_DEPTH_SAME_LAYER_B_A__
        // render B box
        layerB = 0;
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.5f, 0.3f, 0));
        draw(VAO, modelLoc, layerB, model);

        // render A box
        layerA = 0;
        model = glm::mat4(1.0f);
        draw(VAO, modelLoc, layerA, model);
#endif

#ifdef __MODE_SAME_DEPTH_DIFF_LAYER_A_B__
        // render A box
        layerA = 1;
        model = glm::mat4(1.0f);
        draw(VAO, modelLoc, layerA, model);

        // render B box
        layerB = 0;
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.5f, 0.3f, 0));
        draw(VAO, modelLoc, layerB, model);
#endif

#ifdef __MODE_SAME_DEPTH_DIFF_LAYER_B_A__
        // render B box
        layerB = 0;
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.5f, 0.3f, 0));
        draw(VAO, modelLoc, layerB, model);

        // render A box
        layerA = 1;
        model = glm::mat4(1.0f);
        draw(VAO, modelLoc, layerA, model);
#endif

#ifdef __MODE_DIFF_DEPTH_SAME_LAYER_A_B__
        // render A box
        layerA = 0;
        model = glm::mat4(1.0f);
        draw(VAO, modelLoc, layerA, model);

        // render B box
        layerB = 0;
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.5f, 0.3f, 0.2f));
        draw(VAO, modelLoc, layerB, model);
#endif

#ifdef __MODE_DIFF_DEPTH_SAME_LAYER_B_A__
        // render B box
        layerB = 0;
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.5f, 0.3f, 0.2f));
        draw(VAO, modelLoc, layerB, model);

        // render A box
        layerA = 0;
        model = glm::mat4(1.0f);
        draw(VAO, modelLoc, layerA, model);
#endif

#ifdef __MODE_DIFF_DEPTH_DIFF_LAYER_A_B_1__
        // render A box
        layerA = 0;
        model = glm::mat4(1.0f);
        draw(VAO, modelLoc, layerA, model);

        // render B box
        layerB = 1;
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.5f, 0.3f, 0.2f));
        draw(VAO, modelLoc, layerB, model);
#endif

#ifdef __MODE_DIFF_DEPTH_DIFF_LAYER_A_B_2__
        // render A box
        layerA = 1;
        model = glm::mat4(1.0f);
        draw(VAO, modelLoc, layerA, model);

        // render B box
        layerB = 0;
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.5f, 0.3f, 0.2f));
        draw(VAO, modelLoc, layerB, model);
#endif

#ifdef __MODE_DIFF_DEPTH_DIFF_LAYER_B_A_1__
        // render B box
        layerB = 0;
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.5f, 0.3f, 0.2f));
        draw(VAO, modelLoc, layerB, model);

        // render A box
        layerA = 1;
        model = glm::mat4(1.0f);
        draw(VAO, modelLoc, layerA, model);
#endif

#ifdef __MODE_DIFF_DEPTH_DIFF_LAYER_B_A_2__
        // render B box
        layerB = 1;
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.5f, 0.3f, 0.2f));
        draw(VAO, modelLoc, layerB, model);

        // render A box
        layerA = 0;
        model = glm::mat4(1.0f);
        draw(VAO, modelLoc, layerA, model);
#endif

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

void draw(unsigned int VAO, unsigned int modelLoc, int layer, glm::mat4 model)
{
    glPolygonOffset(-1 * (layer + 1), -2 * (layer + 1));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}