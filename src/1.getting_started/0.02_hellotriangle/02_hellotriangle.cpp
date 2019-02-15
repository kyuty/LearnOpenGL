// 02_hellotriangle.cpp : 定义控制台应用程序的入口点。
// VAO VBO

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>


// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;

// Shaders
const GLchar* vertexShaderSource = "#version 330 core\n"			//我们同样显式地表示我们会用core profile功能。
"layout (location = 0) in vec3 position;\n"							/*
																	我们在顶点着色器中声明所有的输入顶点属性，使用in关键字。
																	现在我们只关心位置数据（position），所以我们只需要一个顶点属性（attribute）。
																	创建一个vec3输入变量来表示位置（position）。
																	指定输入变量的location，这是用layout (location = 0)来完成的
																	*/
"void main()\n"
"{\n"
"gl_Position = vec4(position.x, position.y, position.z, 1.0);\n"	/*
																	为了设置顶点着色器的输出，我们必须把位置数据赋值给预定义的gl_Position变量，这个位置数据是一个vec4类型的。
																	在main函数的最后，无论我们给gl_Position设置成什么，它都会成为着色器的输出。
																	由于我们的输入是一个3元素的向量，我们必须把它转换为4元素。
																	我们可以通过把vec3数据作为vec4初始化构造器的参数，同时把w元素设置为1.0f
																	*/
"}\0";
const GLchar* fragmentShaderSource = "#version 330 core\n"
"out vec4 color;\n"
"void main()\n"
"{\n"
"color = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"							/*
																	像素着色器只需要一个输出变量，这个变量是一个4元素表示的最终输出颜色的向量，我们可以自己计算出来。
																	我们可以用out关键字声明输出变量，这里我们命名为color。
																	*/
"}\n\0";

// The MAIN function, from here we start the application and run the game loop
int main()
{
	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	glewInit();

	// Define the viewport dimensions
	glViewport(0, 0, WIDTH, HEIGHT);


	// Build and compile our shader program
	// Vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);				/*
																		我们要做的第一件事是创建一个着色器对象，再次引用它的ID。
																		所以我们储存这个顶点着色器为GLuint，然后用glCreateShader创建着色器
																		把着色器的类型提供glCreateShader作为它的参数。
																		这里我们传递的参数是GL_VERTEX_SHADER这样就创建了一个顶点着色器。
																		*/
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);			//着色器源码附加到着色器对象
																		/*
																		glShaderSource函数把着色器对象作为第一个参数来编译它。
																		第二参数指定了源码中有多少个字符串，这里只有一个。
																		第三个参数是顶点着色器真正的源码，
																		可以把第四个参数设置为NULL。
																		*/
	glCompileShader(vertexShader);										//编译顶点着色器
	// Check for compile time errors
	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// Fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);		
	glCompileShader(fragmentShader);									
	// Check for compile time errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// Link shaders
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);									
																		/*
																		glCreateProgram函数创建一个程序，返回新创建的程序对象的ID引用。
																		现在我们需要把前面编译的着色器附加到程序对象上，
																		然后用glLinkProgram链接它们
																		*/
	// Check for linking errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);


	// Set up vertex data (and buffer(s)) and attribute pointers
	GLfloat vertices[] = {
		-0.5f, -0.5f, 0.0f, // Left  
		0.5f, -0.5f, 0.0f, // Right 
		0.0f,  0.5f, 0.0f  // Top   
	};
	GLuint VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).!!!!!!!!!!
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);	/*
										OpenGL有很多缓冲对象类型，GL_ARRAY_BUFFER是其中一个顶点缓冲对象的缓冲类型
										OpenGL允许我们同时绑定多个缓冲，只要它们是不同的缓冲类型
										*/
										/*
										从这一刻起，我们使用的任何缓冲函数（在GL_ARRAY_BUFFER目标上）都会用来配置当前绑定的缓冲（VBO）。
										然后我们可以调用glBufferData函数，它会把之前定义的顶点数据复制到缓冲的内存中
										*/
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);	/*
																				glBufferData是一个用来把用户定义数据复制到当前绑定缓冲的函数。
																				它的第一个参数是我们希望把数据复制到上面的缓冲类型：顶点缓冲对象当前绑定到GL_ARRAY_BUFFER目标上
																				第二个参数指定我们希望传递给缓冲的数据的大小（字节）:用一个简单的sizeof计算出顶点数据就行
																				第三个参数是我们希望发送的真实数据。
																				第四个参数指定了我们希望显卡如何管理给定的数据。有三种形式：
																					GL_STATIC_DRAW：数据不会或几乎不会改变。
																					GL_DYNAMIC_DRAW：数据会被改变很多。
																					GL_STREAM_DRAW：数据每次绘制时都会改变。
																				三角形的位置数据不会改变，每次渲染调用时都保持原样，所以它使用的类型最好是GL_STATIC_DRAW。
																				如果，比如，一个缓冲中的数据将频繁被改变，那么使用的类型就是GL_DYNAMIC_DRAW或GL_STREAM_DRAW。
																				这样就能确保图形卡把数据放在高速写入的内存部分。
																				*/

																				/*
																				到现在为止,OpenGL还不知道如何解释内存中的顶点数据，以及怎样把顶点数据链接到顶点着色器的属性上。
																				*/
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
																				/*
																				glVertexAttribPointer函数有很多参数，所以我们小心地去了解它们：
																					第一个参数指定我们要配置哪一个顶点属性。
																						记住，我们在顶点着色器中使用layout(location = 0)定义了顶点属性——位置（position）的location。
																						这样要把顶点属性的location设置为0，因为我们希望把数据传递到这个顶点属性中，所以我们在这里填0。
																					第二个参数指定顶点属性的大小。顶点属性是vec3类型，它由3个数值组成。
																					第三个参数指定数据的类型，这里是GL_FLOAT（GLSL中vec*是由浮点数组成的）。
																					第四个参数定义我们是否希望数据被标准化。
																						如果我们设置为GL_TRUE，所有数据都会被映射到0（对于有符号型signed数据是-1）到1之间。
																						我们把它设置为GL_FALSE。
																					第五个参数叫做步长（stride），它告诉我们在连续的顶点属性之间间隔有多少。
																						由于下个位置数据在3个GLfloat后面的位置，我们把步长设置为3 * sizeof(GLfloat)。
																						要注意的是由于我们知道这个数组是紧密排列的（在两个顶点属性之间没有空隙）
																						我们也可以设置为0来让OpenGL决定具体步长是多少（只有当数值是紧密排列时才可用）。
																						每当我们有更多的顶点属性，我们就必须小心地定义每个顶点属性之间的空间。
																					第六个参数有古怪的GLvoid*的强制类型转换。我们的位置数据在缓冲中起始位置的偏移量。由于位置数据是数组的开始，所以这里是0。
																				*/
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind

	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs)

						  // Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();

		// Render
		// Clear the colorbuffer
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Draw our first triangle
		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);		/*
												OpenGL的glDrawArrays函数为我们提供了绘制物体的能力，
												它使用当前激活的着色器、前面定义的顶点属性配置和VBO的顶点数据（通过VAO间接绑定）来绘制基本图形。
												*/
		glBindVertexArray(0);

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}
	// Properly de-allocate all resources once they've outlived their purpose
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return 0;
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}