// Std. Includes
#include <string>
#include <iostream>
#include <fstream>
#include <strstream>

// GLEW
#include <GL\glew.h>

// GLFW
#include <GLFW/glfw3.h>

#include <glut.h>

// GL includes
#include "Shader.h"
#include "Camera.h"

// GLM Mathemtics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Other Libs
#include <SOIL.h>

#include "ExportOBJ.h"
using namespace std;

// Properties
GLuint screenWidth = 800;
GLuint screenHeight = 600;

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_btn_callback(GLFWwindow* window, int button, int action, int mods);
void Do_Movement();

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;
bool CameraRotate = false;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

int drawcallID = 1;

// The MAIN function, from here we start our application and run our Game loop
int main()
{
	// Init GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "Google Earth", nullptr, nullptr); // Windowed
	glfwMakeContextCurrent(window);

	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetMouseButtonCallback(window, mouse_btn_callback);

	// Options
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Initialize GLEW to setup the OpenGL Function pointers
	glewExperimental = GL_TRUE;
	glewInit();

	// Define the viewport dimensions
	glViewport(0, 0, screenWidth, screenHeight);

	// Setup some OpenGL options
	glEnable(GL_DEPTH_TEST);

	// Setup and compile our shaders
	Shader ourShader("./shader.vs", "./shader.frag");

	strstream drawcallID_stream;
	string drawcall_str;
	drawcallID_stream << drawcallID;
	drawcallID_stream >> drawcall_str;
	string folderstr = "./drawcall/" + drawcall_str;
	vector<GLfloat> vertices;
	// read from file
	ifstream vertex_file(folderstr + "/Vertex.txt");
	//int i = 0;
	while (vertex_file)
	{
		int line;
		GLfloat data;
		vertex_file >> line;
		vertex_file >> data;
		vertices.push_back(data);
		//cout << line << " " << vertices[i] << endl;
		//i++;
	}
	vertex_file.close();

	vector<GLuint> indices;
	ifstream index_file(folderstr + "/Index.txt");
	while (index_file) {
		int line;
		GLuint index;
		index_file >> line;
		index_file >> index;
		indices.push_back(index);
	}
	index_file.close();

	ifstream offset_file(folderstr + "/offset.txt");
	int offset1, offset2;
	while (offset_file) {
		offset_file >> offset1;
		offset_file >> offset2;
	}
	offset_file.close();

	exportOBJ(vertices, indices, offset1 / 4, offset2 / 4, drawcall_str);

	GLuint VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// Bind our Vertex Array Object first, then bind and set our buffers and pointers.
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

	// position(xyz) attribute
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// cup normal attribute
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 1 * sizeof(GLfloat), (GLvoid*)(offset1));
	glEnableVertexAttribArray(1);
	// cup texcoord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)(offset2));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);  // unbind VBO
	glBindVertexArray(0); // Unbind VAO ²»Òªunbind EBO

						  // Load and create a texture 
	GLuint texture1;
	// --== TEXTURE 1 == --
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object
											// Set our texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// Set texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Load, create texture and generate mipmaps
	int width, height;
	string img_file = folderstr + "/Texture.bmp";
	unsigned char* image = SOIL_load_image(img_file.c_str(), &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.


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
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Draw our first triangle
		ourShader.Use();

		// Bind Textures using texture units
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glUniform1i(glGetUniformLocation(ourShader.Program, "ourTexture1"), 0);

		// Create camera transformation
		glm::mat4 view;
		view = camera.GetViewMatrix();
		glm::mat4 projection;
		projection = glm::perspective(camera.Zoom, (float)screenWidth / (float)screenHeight, 0.1f, 1000.0f);
		glm::mat4 model;
		model = glm::translate(model, glm::vec3(0, 0, 0));
		model = glm::rotate(model, -90.0f, glm::vec3(0, 1.0f, 0.0f));
		
		// Get the uniform locations
		GLint modelLoc = glGetUniformLocation(ourShader.Program, "model");
		GLint viewLoc = glGetUniformLocation(ourShader.Program, "view");
		GLint projLoc = glGetUniformLocation(ourShader.Program, "projection");
		// Pass the matrices to the shader
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glBindVertexArray(VAO);
		//glDrawArrays(GL_TRIANGLES, 0, vertices.size());
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// Swap the buffers
		glfwSwapBuffers(window);
	}
	// Properly de-allocate all resources once they've outlived their purpose
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glfwTerminate();
	return 0;
}

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
	//cout << key << endl;
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
	if (CameraRotate) {
		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		GLfloat xoffset = xpos - lastX;
		GLfloat yoffset = lastY - ypos;  // Reversed since y-coordinates go from bottom to left

		lastX = xpos;
		lastY = ypos;

		camera.ProcessMouseMovement(xoffset, yoffset);
	}
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

void mouse_btn_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) {
		CameraRotate = true;
	}
	else
		CameraRotate = false;
}