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
#include <cglm/struct.h>
#include <camera.h>
#include <Windows.h>

/////////////////////////////////////////////////////////////////////////////////
// Globals

Camera cam = {
	.front = { 0.0f, 0.0f, -1.0f },
	.pos = { 0.0f, 1.0f, 3.0f },
	.up = { 0.0f, 1.0f, 0.0f },
	.speed = 2.5f,
	.pitch = 0.0f,
	.yaw = -90.0f,
	.fov = 95.0f,
	.sens = 0.1f,
	.lastMouseX = 300.0f,
	.lastMouseY = 400.0f,
	.firstMouse = true,
	.yVelocity = 0.0f,
	.worldUp = { 0.0f, 1.0f, 0.0f },
	.right = { 1.0f, 0.0f, 0.0f }
};
float g_width = WINDOW_WIDTH;
float g_height = WINDOW_HEIGHT;

/////////////////////////////////////////////////////////////////////////////////
// Callbacks

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	g_width = width;
	g_height = height;
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
	if (cam.fov < FOV_MIN)
		cam.fov = FOV_MIN;
	if (cam.fov > FOV_MAX)
		cam.fov = FOV_MAX;
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
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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
	if (nrChannels == 3)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imgData);
	else if (nrChannels == 4)
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
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 5 * sizeof(float), 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, false, 5 * sizeof(float), (3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}

/////////////////////////////////////////////////////////////////////////////////
// Draws

// 0 -> front
// 1 -> back
// 2 -> left
// 3 -> right
// 4 -> down
// 5 -> up
void drawFullCube(unsigned int* textures, unsigned int program, mat4s mvp, unsigned int mvpLoc, unsigned int VAOcube) {
	glUseProgram(program);
	glUniformMatrix4fv(mvpLoc, 1, false, mvp.raw); 
	glBindVertexArray(VAOcube);
	for (size_t i = 0; i < 6; i++) {
		glBindTexture(GL_TEXTURE_2D, textures[i]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices) / 6, cubeVertices + 30 * i, GL_STATIC_DRAW);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
	glUseProgram(0);
}
void drawFullCubes(vec3s cubes[], size_t cubeLen, unsigned int* textures, unsigned int program, mat4s mvp, unsigned int mvpLoc, unsigned int VAOcube) {
	// translate by position, then negate back to start
	mat4s start = mvp;
	for (size_t i = 0; i < cubeLen; i++) {
		mvp = glms_translate(start, cubes[i]);
		drawFullCube(textures, program, mvp, mvpLoc, VAOcube);
	}
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
	int grassTopW, grassTopH, grassTopChannels;
	int grassSideW, grassSideH, grassSideChannels;
	int dirtW, dirtH, dirtChannels;
	char* imgGrassTop = stbi_load("images\\grass_top.png", &grassTopW, &grassTopH, &grassTopChannels, 0);
	char* imgGrassSide = stbi_load("images\\grass_side.png", &grassSideW, &grassSideH, &grassSideChannels, 0);
	char* imgDirt = stbi_load("images\\dirt.png", &dirtW, &dirtH, &dirtChannels, 0);
	if (!imgGrassTop || !imgGrassSide || !imgDirt) {
		printf("Could not load image.");
		return;
	}
	unsigned int grassTopTex = genBindStdTexture(imgGrassTop, grassTopW, grassTopH, grassTopChannels);
	unsigned int grassSideTex = genBindStdTexture(imgGrassSide, grassSideW, grassSideH, grassSideChannels);
	unsigned int dirtTex = genBindStdTexture(imgDirt, dirtW, dirtH, dirtChannels);

	// uniform locations
	unsigned int mvpLoc = glGetUniformLocation(cubeProgram, "mvp");
	unsigned int textureSide = glGetUniformLocation(cubeProgram, "textureSide");

	// z buffer
	glEnable(GL_DEPTH_TEST);

	// render variables
	float lastFrame = 0.0f;
	unsigned int grassTextures[6] = {
		grassSideTex,
		grassSideTex,
		grassSideTex,
		grassSideTex,
		dirtTex,
		grassTopTex
	};

	// cubes example
	vec3s cubes[100];
	for (int x = 0; x < 10; x++)
		for (int z = 0; z < 10; z++)
			cubes[10 * x + z] = (vec3s){ x, 0.0f, z };

	// render loop
	while (!glfwWindowShouldClose(window)) {
		float frameDelta = updateFrameDelta(&lastFrame);

		// update color
		glClearColor(0.1f, 0.4f, 0.55f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// mvp
		mat4s model = glms_translate_make((vec3s) { -5.0f, -1.0f, -5.0f });
		mat4s view = glms_lookat(cam.pos, glms_vec3_add(cam.pos, cam.front), cam.up);
		mat4s perspective = glms_perspective(glm_rad(cam.fov), g_width / g_height, NEAR_PLANE, FAR_PLANE);
		mat4s mvp = glms_mul(glms_mul(perspective, view), model);

		processInput(window, frameDelta);
		drawFullCubes(cubes, 100, grassTextures, cubeProgram, mvp, mvpLoc, VAOcube);

		glfwPollEvents();
		glfwSwapBuffers(window);
		Sleep(1);
	}

	glfwTerminate();
}