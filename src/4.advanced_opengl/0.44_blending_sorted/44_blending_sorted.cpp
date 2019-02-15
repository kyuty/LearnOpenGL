// 44_blending_sorted.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
// Std. Includes
#include <string>
#include <map>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// GL includes
#include "Shader.h"
#include "Camera.h"

// GLM Mathemtics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Other Libs
#include <SOIL.h>

// Properties
GLuint screenWidth = 800, screenHeight = 600;

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void Do_Movement();
GLuint loadTexture(GLchar* path, GLboolean alpha = false);

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

// 对矩阵求逆，结果保存在a中
void brinv(double a[], int n)
{
	int *is, *js, i, j, k, l, u, v;
	double d, p;
	is = new int[n];
	js = new int[n];
	for (k = 0; k <= n - 1; k++)
	{
		d = 0.0;
		for (i = k; i <= n - 1; i++)
			for (j = k; j <= n - 1; j++)
			{
				l = i*n + j; p = fabs(a[l]);
				if (p>d) { d = p; is[k] = i; js[k] = j; }
			}
		if (d + 1.0 == 1.0)
		{
			free(is); free(js); printf("err**not inv\n");
			return;
		}
		if (is[k] != k)
			for (j = 0; j <= n - 1; j++)
			{
				u = k*n + j; v = is[k] * n + j;
				p = a[u]; a[u] = a[v]; a[v] = p;
			}
		if (js[k] != k)
			for (i = 0; i <= n - 1; i++)
			{
				u = i*n + k; v = i*n + js[k];
				p = a[u]; a[u] = a[v]; a[v] = p;
			}
		l = k*n + k;
		a[l] = 1.0 / a[l];
		for (j = 0; j <= n - 1; j++)
			if (j != k)
			{
				u = k*n + j; a[u] = a[u] * a[l];
			}
		for (i = 0; i <= n - 1; i++)
			if (i != k)
				for (j = 0; j <= n - 1; j++)
					if (j != k)
					{
						u = i*n + j;
						a[u] -= a[i*n + k] * a[k*n + j];
					}
		for (i = 0; i <= n - 1; i++)
			if (i != k)
			{
				u = i*n + k;
				a[u] = -a[u] * a[l];
			}
	}
	for (k = n - 1; k >= 0; k--)
	{
		if (js[k] != k)
			for (j = 0; j <= n - 1; j++)
			{
				u = k*n + j; v = js[k] * n + j;
				p = a[u]; a[u] = a[v]; a[v] = p;
			}
		if (is[k] != k)
			for (i = 0; i <= n - 1; i++)
			{
				u = i*n + k; v = i*n + is[k];
				p = a[u]; a[u] = a[v]; a[v] = p;
			}
	}
	free(is); free(js);
}

void frustumM(float* m, int offset, float left, float right, float bottom, float top, float near, float far)
{
	/*
	float l = (float)(-Math.tan(Math.toRadians((double)this.left))) * near;
	float r = (float)Math.tan(Math.toRadians((double)this.right)) * near;
	float b = (float)(-Math.tan(Math.toRadians((double)this.bottom))) * near;
	float t = (float)Math.tan(Math.toRadians((double)this.top)) * near;

	left = -1.0f * tan(left / 180.0f * 3.141592654f) * near;
	right = 1.0f * tan(right / 180.0f * 3.141592654f) * near;
	bottom = -1.0f * tan(bottom / 180.0f * 3.141592654f) * near;
	top = 1.0f * tan(top / 180.0f * 3.141592654f) * near;
	*/

	const float r_width = 1.0f / (right - left);
	const float r_height = 1.0f / (top - bottom);
	const float r_depth = 1.0f / (near - far);
	const float x = 2.0f * (near * r_width);
	const float y = 2.0f * (near * r_height);
	const float A = 2.0f * ((right + left) * r_width);
	const float B = (top + bottom) * r_height;
	const float C = (far + near) * r_depth;
	const float D = 2.0f * (far * near * r_depth);
	m[offset + 0] = x;
	m[offset + 5] = y;
	m[offset + 8] = A;
	m[offset + 9] = B;
	m[offset + 10] = C;
	m[offset + 14] = D;
	m[offset + 11] = -1.0f;
	m[offset + 1] = 0.0f;
	m[offset + 2] = 0.0f;
	m[offset + 3] = 0.0f;
	m[offset + 4] = 0.0f;
	m[offset + 6] = 0.0f;
	m[offset + 7] = 0.0f;
	m[offset + 12] = 0.0f;
	m[offset + 13] = 0.0f;
	m[offset + 15] = 0.0f;
}

// The MAIN function, from here we start our application and run our Game loop
int main()
{
	// Init GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "LearnOpenGL", nullptr, nullptr); // Windowed
	glfwMakeContextCurrent(window);


	glm::mat4 stereoMat = glm::rotate(glm::mat4(), -0.06f, glm::vec3(0.0f, 1.0f, 0.0f));
	printf("stereoMat = %d\n", stereoMat.length());
	for (int i = 0; i < stereoMat.length(); i++) {
		for (int j = 0; j < 4; j++) {
			printf("%f ", stereoMat[i][j]);
		}
		printf("\n");
	}
	glm::mat4 stereoMat1 = glm::rotate(glm::mat4(), 0.06f, glm::vec3(0.0f, 1.0f, 0.0f));
	printf("stereoMat1 = %d\n", stereoMat1.length());
	for (int i = 0; i < stereoMat1.length(); i++) {
		for (int j = 0; j < 4; j++) {
			printf("%f ", stereoMat1[i][j]);
		}
		printf("\n");
	}

	double m[] = {
		1.0f,0.0f,0.0f,0.0f,
		0.0f,1.0f,0.0f,0.0f,
		0.0f,0.0f,1.0f,0.0f,
		0.0f,0.0f,0.0f,1.0f
	};

	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (GLfloat)1920 / (GLfloat)1080, 0.1f, 100.0f);
	printf("projection = \n");
	for (int i = 0; i < projection.length(); i++) {
		for (int j = 0; j < 4; j++) {
			printf("%f ", projection[i][j]);
			m[i * 4 + j] = projection[i][j];
		}
		printf("\n");
	}

	glm::mat4 projection1 = glm::perspective(glm::radians(45.0f), (GLfloat)1920 / (GLfloat)1080, 0.1f, 100.0f);
	printf("projection1 = \n");
	for (int i = 0; i < projection1.length(); i++) {
		for (int j = 0; j < 4; j++) {
			printf("%f ", projection1[i][j]);
		}
		printf("\n");
	}

	brinv(m, 4);
	glm::mat4 inveser;
	printf("projection_inveser = \n");
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {

			printf("%f ", m[i * 4 + j]);
			inveser[i][j] = m[i * 4 + j];
		}
		printf("\n");
	}

	glm::mat4 ans = projection1 * inveser;
	printf("projection1 * inveser (guilu)= \n");
	for (int i = 0; i < ans.length(); i++) {
		for (int j = 0; j < 4; j++) {
			printf("%f ", ans[i][j]);
		}
		printf("\n");
	}

	glm::mat4 ans_new = projection * ans;
	printf("ans_new (must be projection1)= \n");
	for (int i = 0; i < ans_new.length(); i++) {
		for (int j = 0; j < 4; j++) {
			printf("%f ", ans_new[i][j]);
		}
		printf("\n");
	}


	{
		double m[] = {
			1.0f,0.0f,0.0f,0.0f,
			0.0f,1.0f,0.0f,0.0f,
			0.0f,0.0f,1.0f,0.0f,
			0.0f,0.0f,0.0f,1.0f
		};

		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (GLfloat)800 / (GLfloat)400, 0.1f, 100.0f);
		printf("projection = \n");
		for (int i = 0; i < projection.length(); i++) {
			for (int j = 0; j < 4; j++) {
				printf("%f ", projection[i][j]);
				m[i * 4 + j] = projection[i][j];
			}
			printf("\n");
		}

		glm::mat4 projection1 = glm::perspective(glm::radians(45.0f), (GLfloat)800 / (GLfloat)400, 0.1f, 1000.0f);
		printf("projection_1 = \n");
		for (int i = 0; i < projection1.length(); i++) {
			for (int j = 0; j < 4; j++) {
				printf("%f ", projection1[i][j]);
			}
			printf("\n");
		}

		brinv(m, 4);
		glm::mat4 inveser;
		printf("projection_inveser = \n");
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				printf("%f ", m[i * 4 + j]);
				inveser[i][j] = m[i * 4 + j];
			}
			printf("\n");
		}

		glm::mat4 ans = projection1 * inveser;
		printf("projection1 * inveser (guilu)= \n");
		for (int i = 0; i < ans.length(); i++) {
			for (int j = 0; j < 4; j++) {
				printf("%f ", ans[i][j]);
			}
			printf("\n");
		}

		glm::mat4 ans_new = projection * ans;
		printf("ans_new (must be projection1)= \n");
		for (int i = 0; i < ans_new.length(); i++) {
			for (int j = 0; j < 4; j++) {
				printf("%f ", ans_new[i][j]);
			}
			printf("\n");
		}

	}

	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// Options
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Initialize GLEW to setup the OpenGL Function pointers
	glewExperimental = GL_TRUE;
	glewInit();

	// Define the viewport dimensions
	glViewport(0, 0, screenWidth, screenHeight);

	// Setup some OpenGL options
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Setup and compile our shaders
	Shader shader("shaders/blending_sorted.vert", "shaders/blending_sorted.frag");

#pragma region "object_initialization"
	// Set the object data (buffers, vertex attributes)
	GLfloat cubeVertices[] = {
		// Positions          // Texture Coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  1.0f,  1.0f,
		0.5f,  0.5f, -0.5f,  1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  1.0f,  1.0f,
		0.5f,  0.5f,  0.5f,  1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f,  0.0f,

		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  1.0f,  1.0f,
		0.5f, -0.5f, -0.5f,  0.0f,  1.0f,
		0.5f, -0.5f, -0.5f,  0.0f,  1.0f,
		0.5f, -0.5f,  0.5f,  0.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f,  1.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  1.0f,
		0.5f, -0.5f,  0.5f,  1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,
		0.5f,  0.5f, -0.5f,  1.0f,  1.0f,
		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f
	};
	GLfloat planeVertices[] = {
		// Positions          // Texture Coords (note we set these higher than 1 that together with GL_REPEAT as texture wrapping mode will cause the floor texture to repeat)
		5.0f, -0.5f,  5.0f,  2.0f,  0.0f,
		-5.0f, -0.5f,  5.0f,  0.0f,  0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f,  2.0f,

		5.0f, -0.5f,  5.0f,  2.0f,  0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f,  2.0f,
		5.0f, -0.5f, -5.0f,  2.0f,  2.0f
	};
	GLfloat transparentVertices[] = {
		// Positions         // Texture Coords (swapped y coordinates because texture is flipped upside down)
		0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
		0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
		1.0f, -0.5f,  0.0f,  1.0f,  1.0f,

		0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
		1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
		1.0f,  0.5f,  0.0f,  1.0f,  0.0f
	};
	// Setup cube VAO
	GLuint cubeVAO, cubeVBO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);
	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glBindVertexArray(0);
	// Setup plane VAO
	GLuint planeVAO, planeVBO;
	glGenVertexArrays(1, &planeVAO);
	glGenBuffers(1, &planeVBO);
	glBindVertexArray(planeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glBindVertexArray(0);
	// Setup transparent plane VAO
	GLuint transparentVAO, transparentVBO;
	glGenVertexArrays(1, &transparentVAO);
	glGenBuffers(1, &transparentVBO);
	glBindVertexArray(transparentVAO);
	glBindBuffer(GL_ARRAY_BUFFER, transparentVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(transparentVertices), transparentVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glBindVertexArray(0);

	// Load textures
	GLuint cubeTexture = loadTexture("textures/black_tiles.jpg");
	GLuint floorTexture = loadTexture("textures/metal.png");
	GLuint transparentTexture = loadTexture("textures/window.png", true);
#pragma endregion

	std::vector<glm::vec3> windows;
	windows.push_back(glm::vec3(-1.5f, 0.0f, -0.48f));
	windows.push_back(glm::vec3(1.5f, 0.0f, 0.51f));
	windows.push_back(glm::vec3(0.0f, 0.0f, 0.7f));
	windows.push_back(glm::vec3(-0.3f, 0.0f, -2.3f));
	windows.push_back(glm::vec3(0.5f, 0.0f, -0.6f));

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Set frame time
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Check and call events
		glfwPollEvents();
		Do_Movement();

		// Clear the colorbuffer
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Sort windows
		std::map<GLfloat, glm::vec3> sorted;
		for (GLuint i = 0; i < windows.size(); i++)
		{
			GLfloat distance = glm::length(camera.Position - windows[i]);
			sorted[distance] = windows[i];
		}

		// Draw objects
		shader.Use();
		glm::mat4 model;
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(camera.Zoom, (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
		/*
		printf("projection = \n");
		for (int i = 0; i < projection.length(); i++) {
		for (int j = 0; j < 4; j++) {
		printf("%f ", projection[i][j]);
		}
		printf("\n");
		}
		printf("camera = \n");
		for (int i = 0; i < view.length(); i++) {
		for (int j = 0; j < 4; j++) {
		printf("%f ", view[i][j]);
		}
		printf("\n");
		}
		*/
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		// Cubes
		glBindVertexArray(cubeVAO);
		glBindTexture(GL_TEXTURE_2D, cubeTexture);  // We omit the glActiveTexture part since TEXTURE0 is already the default active texture unit. (a single sampler used in fragment is set to 0 as well by default)		
		model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
		/*
		printf("model = \n");
		for (int i = 0; i < model.length(); i++) {
		for (int j = 0; j < 4; j++) {
		printf("%f ", model[i][j]);
		}
		printf("\n");
		}
		glm::mat4 res = projection * view * model;
		printf("projection * camera * model = \n");
		for (int i = 0; i < res.length(); i++) {
		for (int j = 0; j < 4; j++) {
		printf("%f ", res[i][j]);
		}
		printf("\n");
		}

		glm::mat4 camera_model = view * model;
		printf("camera * model = \n");
		for (int i = 0; i < camera_model.length(); i++) {
		for (int j = 0; j < 4; j++) {
		printf("%f ", camera_model[i][j]);
		}
		printf("\n");
		}

		glm::mat4 res2 = projection * camera_model;
		printf("projection * camera_model = \n");
		for (int i = 0; i < res2.length(); i++) {
		for (int j = 0; j < 4; j++) {
		printf("%f ", res2[i][j]);
		}
		printf("\n");
		}
		*/

		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 36);
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 36);
		// Floor
		glBindVertexArray(planeVAO);
		glBindTexture(GL_TEXTURE_2D, floorTexture);
		model = glm::mat4();
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// Render windows (from furthest to nearest)
		glBindVertexArray(transparentVAO);
		glBindTexture(GL_TEXTURE_2D, transparentTexture);
		for (std::map<float, glm::vec3>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it)
		{
			model = glm::mat4();
			model = glm::translate(model, it->second);
			glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
		glBindVertexArray(0);


		// Swap the buffers
		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}

// This function loads a texture from file. Note: texture loading functions like these are usually 
// managed by a 'Resource Manager' that manages all resources (like textures, models, audio). 
// For learning purposes we'll just define it as a utility function.
GLuint loadTexture(GLchar* path, GLboolean alpha)
{
	//Generate texture ID and load texture data 
	GLuint textureID;
	glGenTextures(1, &textureID);
	int width, height;
	unsigned char* image = SOIL_load_image(path, &width, &height, 0, alpha ? SOIL_LOAD_RGBA : SOIL_LOAD_RGB);
	// Assign texture to ID
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, alpha ? GL_RGBA : GL_RGB, width, height, 0, alpha ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, alpha ? GL_CLAMP_TO_EDGE : GL_REPEAT);	// Use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes value from next repeat 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, alpha ? GL_CLAMP_TO_EDGE : GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	SOIL_free_image_data(image);
	return textureID;

}

#pragma region "User input"

// Moves/alters the camera positions based on user input
void Do_Movement()
{
	// Camera controls
	if (keys[GLFW_KEY_W])
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (keys[GLFW_KEY_S])
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (keys[GLFW_KEY_A])
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (keys[GLFW_KEY_D])
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
}


void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

#pragma endregion