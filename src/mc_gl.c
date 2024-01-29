#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <glad/glad.h>
#include <glfw3.h>
#include <mc_gl.h>
#include <shaders.h>
#include <unistd.h>
#include <string.h>
#include <stb_image.h>
#include <Windows.h>
#include <cglm/struct.h>
#include <camera.h>

/////////////////////////////////////////////////////////////////////////////////
// Globals

Camera cam = {
	.front = { 0.0f, 0.0f, -1.0f },
	.pos = { 0.0f, 1.0f, 3.0f },
	.up = { 0.0f, 1.0f, 0.0f },
	.speed = 2.5f,
	.pitch = 0.0f,
	.yaw = -90.0f,
	.fov = 75.0f,
	.sens = 0.1f,
	.lastMouseX = 300.0f,
	.lastMouseY = 400.0f,
	.firstMouse = true,
	.yVelocity = 0.0f,
	.worldUp = { 0.0f, 1.0f, 0.0f },
	.right = { 1.0f, 0.0f, 0.0f }
};

/////////////////////////////////////////////////////////////////////////////////
// Callbacks

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}
void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
	if (cam.firstMouse) {
		cam.lastMouseX = xpos;
		cam.lastMouseY = ypos;
		cam.firstMouse = false;
	}

	float xOffset = xpos - cam.lastMouseX;
	float yOffset = cam.lastMouseY - ypos;
	cam.lastMouseX = xpos;
	cam.lastMouseY = ypos;

	xOffset *= cam.sens;
	yOffset *= cam.sens;
	
	cam.yaw += xOffset;
	cam.pitch += yOffset;

	// limit pitch
	if (cam.pitch > 89.0f)
		cam.pitch = 89.0f;
	if (cam.pitch < -89.0f)
		cam.pitch = -89.0f;
}
void scrollCallBack(GLFWwindow* window, double xoffset, double yoffset) {
	cam.fov -= (float)yoffset;
	if (cam.fov < 45.0f)
		cam.fov = 45.0f;
	if (cam.fov > 120.0f)
		cam.fov = 120.0f;
}

/////////////////////////////////////////////////////////////////////////////////
// Utils

void processInput(GLFWwindow* window, float frameDelta) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		cam_jump(&cam);
	cam_updatePos(&cam, window, frameDelta);
	cam_updateVectors(&cam);
}
GLFWwindow* initWindow(int width, int height) {
	// glfw
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE, 3);

	// create window object
	GLFWwindow* window = glfwCreateWindow(width, height, "GL MC", NULL, NULL);
	if (window == NULL) {
		printf("Failed to create GLFW window\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		printf("Failed to initialize GLAD\n");
		return -1;
	}
	return window;
}
unsigned int genBindVAO(unsigned int VBO, float vertices[], size_t size) {
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);

	return VAO;
}
unsigned int genBindVBO(float vertices[], size_t size) {
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);

	return VBO;
}
unsigned int genBindEBO(float vertices[], unsigned int indices[], size_t iSize) {
	unsigned int EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, iSize, indices, GL_STATIC_DRAW);

	return EBO;
}
void textureRectanglePointerArithmetic() {
	// pointer arithmetic for attributes
	// pos
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color
	glVertexAttribPointer(1, 3, GL_FLOAT, false, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texture coord
	glVertexAttribPointer(2, 2, GL_FLOAT, false, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
}
unsigned int genBindStdTexture(char* imgData, int width, int height, int nrChannels) {
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	// texture wrapping
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// create texture and free data
	// RGB for 3 channels and RGBA for 4 channels
	if(nrChannels == 3)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imgData);
	else if(nrChannels == 4)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imgData);
	glGenerateMipmap(GL_TEXTURE_2D);
	free(imgData);
	return texture;
}
float updateFrameDelta(float* lastFrame) {
	// update frame time
	float currFrame = glfwGetTime();
	float deltaTime = currFrame - *lastFrame;
	*lastFrame = currFrame;
	return deltaTime;
}
void setStdCubePointerArithmetic() {
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, false, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}

/////////////////////////////////////////////////////////////////////////////////
// Main function

void mc_gl() {
	GLFWwindow* window = initWindow(WINDOW_WIDTH, WINDOW_HEIGHT);

	// capture cursor
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// set callbacks
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	glfwSetScrollCallback(window, scrollCallBack);

	unsigned int VBOs[2];
	glGenBuffers(2, &VBOs);

	// cube
	unsigned int VAOcube = genBindVAO(VBOs[0], cubeVertices, sizeof(cubeVertices));
	setStdCubePointerArithmetic();

	unsigned int cubeProgram = buildShaderProgram("spin_cube_mvp.vert", "spin_cube.frag");

	// textures
	stbi_set_flip_vertically_on_load(true);
	unsigned int grassTopW, grassTopH, grassTopChannels;
	unsigned int grassSideW, grassSideH, grassSideChannels;
	byte* imgGrassTop = stbi_load("images\\grass_top.png", &grassTopW, &grassTopH, &grassTopChannels, 0);
	byte* imgGrassSide = stbi_load("images\\grass_side.png", &grassSideW, &grassSideH, &grassSideChannels, 0);
	if (!imgGrassTop || !imgGrassSide) {
		printf("Could not load image.");
		return;
	}
	unsigned int grassTopTex = genBindStdTexture(imgGrassTop, grassTopW, grassTopH, grassTopChannels);
	unsigned int grassSideTex = genBindStdTexture(imgGrassSide, grassSideW, grassSideH, grassSideChannels);

	// uniform locations
	unsigned int mvpLoc = glGetUniformLocation(cubeProgram, "mvp");
	unsigned int textureSide = glGetUniformLocation(cubeProgram, "textureSide");

	// z buffer
	glEnable(GL_DEPTH_TEST);

	// render variables
	float lastFrame = 0.0f;

	// render loop
	while (!glfwWindowShouldClose(window)) {
		float frameDelta = updateFrameDelta(&lastFrame);
		processInput(window, frameDelta);

		// update color
		glClearColor(0.1f, 0.4f, 0.55f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// mvp
		mat4s model = glms_translate_make((vec3s) { -10.0f, -1.0f, -10.0f });
		mat4s view = glms_lookat(cam.pos, glms_vec3_add(cam.pos, cam.front), cam.up);
		mat4s perspective = glms_perspective(glm_rad(cam.fov), WINDOW_WIDTH / WINDOW_HEIGHT, NEAR_PLANE, FAR_PLANE);
		mat4s mvp = glms_mul(glms_mul(perspective, view), model);

		for (int x = 0; x < 20; x++)	{
			mvp = glms_translate(mvp, (vec3s) { 1.0f, 0.0f, 0.0f });
			for (int z = 0; z < 20; z++) {
				mvp = glms_translate(mvp, (vec3s) { 0.0f, 0.0f, 1.0f });
				for (int side = 0; side < 6; side++) {
					glUseProgram(cubeProgram);
					if (side == UP)
						glBindTexture(GL_TEXTURE_2D, grassTopTex);
					else
						glBindTexture(GL_TEXTURE_2D, grassSideTex);
					glUniformMatrix4fv(mvpLoc, 1, false, mvp.raw);
					glBindVertexArray(VAOcube);
					glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices) / 6, cubeVertices + 30 * side, GL_STATIC_DRAW);
					glDrawArrays(GL_TRIANGLES, 0, 6);
				}
			}
			mvp = glms_translate(mvp, (vec3s) { 0.0f, 0.0f, -20.0f });
		}

		glfwPollEvents();
		glfwSwapBuffers(window);

		Sleep(1);
	}

	glfwTerminate();
}