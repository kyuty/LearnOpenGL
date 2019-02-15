#include <iostream>

// 如果你使用GLEW的静态库，我们必须在包含GLEW之前定义一个预处理变量GLEW_STATIC
// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// 必须在GLFW之前引入GLEW。因为GLEW的头文件已经包含了OpenGL的头文件（GL/gl.h），所以要在其他头文件之前引入GLEW，因为它们需要有OpenGL才能起作用。

// GLFW
#include <GLFW/glfw3.h>


// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;


// The MAIN function, from here we start the application and run the game loop
int main()
{
	std::cout << "Starting GLFW context, OpenGL 3.3" << std::endl;
	// Init GLFW
	glfwInit();														//初始化GLFW
	// Set all the required options for GLFW
	//告诉GLFW我们使用的OpenGL版本是3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);					//主版本号
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);					//次版本号
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);	//明确地使用core - profile(核心配置)
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);						//不可以调整窗口大小
																	/*
																	显式地告诉GLFW我们希望是用core-profile会导致当我们调用一个OpenGL的遗留函数会产生invalid operation错误，
																	当我们意外地使用了不该使用的旧函数时它是一个很好的提醒
																	*/
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); 			// uncomment this statement to fix compilation on OS X
#endif
																	/*
																	注意在Mac OS X上在初始化代码里你同样需要添加glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);才能工作
																	*/

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);

	// 我们希望在调用任何OpenGL函数前初始化GLEW ================================= first
	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}
	// 我们希望在调用任何OpenGL函数前初始化GLEW ================================= end

	// Define the viewport dimensions
	glViewport(0, 0, WIDTH, HEIGHT);

	const GLubyte *OpenGLVersion = glGetString(GL_VERSION); //返回当前OpenGL实现的版本号
	/*打印版本号*/

	/*
	我们不希望应用绘制了一个图像之后立即退出，然后关闭窗口。
	我们想让应用持续地绘制图像，监听用户输入直到软件被明8确告知停止。
	为了达到这个目的，我们必须创建一个while循环，现在我们调用游戏循环（game loop）
	这样，在我们告诉GLFW停止之前应用就会一直保持运行状态
	*/
	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		// 检查是否有key事件，鼠标事件等。若有则回调相关方法
		glfwPollEvents();

		// Render
		// Clear the colorbuffer
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}

	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return 0;
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	std::cout << key << std::endl;
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}