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
#include <chunk.h>

/////////////////////////////////////////////////////////////////////////////////
// Globals

Camera cam = {
	.front = {{ 0.0f, 0.0f, -1.0f }},
	.pos = {{ 0.0f, 1.0f, 3.0f }},
	.up = {{ 0.0f, 1.0f, 0.0f }},
	.speed = 2.5f,
	.pitch = 0.0f,
	.yaw = -90.0f,
	.fov = 95.0f,
	.sens = 0.1f,
	.lastMouseX = 300.0f,
	.lastMouseY = 400.0f,
	.firstMouse = true,
	.yVelocity = 0.0f,
	.worldUp = {{ 0.0f, 1.0f, 0.0f }},
	.right = {{ 1.0f, 0.0f, 0.0f }}
};
float g_width = WINDOW_WIDTH;
float g_height = WINDOW_HEIGHT;

/////////////////////////////////////////////////////////////////////////////////
// Callbacks

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
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
#pragma GCC diagnostic pop

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
	GLFWwindow* window = glfwCreateWindow(width, height, "voxel-game", NULL, NULL);
	if (window == NULL) {
		printf("Failed to create GLFW window\n");
		glfwTerminate();
		return NULL;
	}
	glfwMakeContextCurrent(window);

	// glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		printf("Failed to initialize GLAD\n");
		return NULL;
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
inline float updateFrameDelta(float* lastFrame) {
	// update frame time
	float currFrame = glfwGetTime();
	float deltaTime = currFrame - *lastFrame;
	*lastFrame = currFrame;
	return deltaTime;
}
void setStdCubePointerArithmetic() {
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 5 * sizeof(float), 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, false, 5 * sizeof(float), (void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}
inline void updateFramerate(GLFWwindow* window) {
	static int fps = 0;
	static float prevTime = 0.0f;
	float currTime = glfwGetTime();
	fps++;
	if (currTime - prevTime >= 1.0f) {
		prevTime++;
		char str[6];
		sprintf(str, "%d", fps);
		glfwSetWindowTitle(window, str);
		fps = 0;
	}
}

/////////////////////////////////////////////////////////////////////////////////
// Draws

// 0 -> front
// 1 -> back
// 2 -> left
// 3 -> right
// 4 -> down
// 5 -> up
void drawCube(unsigned int* textures, Chunk* c, ivec3s pos, unsigned int program, mat4s mvp, unsigned int mvpLoc, unsigned int VAOcube) {
	glUseProgram(program);
	glUniformMatrix4fv(mvpLoc, 1, false, *mvp.raw);
	glBindVertexArray(VAOcube);
	
	for (size_t i = 0; i < 6; i++) {
		// check adjacent side for existing cube
		// if 0 byte -> air block -> draw
		// else do not draw, because side is obscured by adjacent block
		ivec3s adj = {{ 0, 0, 0 }};
		glm_ivec3_add(pos.raw, DIR_VECS[i], adj.raw);

		if (0 <= adj.x && adj.x < CHUNK_SIZE &&
			0 <= adj.y && adj.y < CHUNK_SIZE &&
			0 <= adj.z && adj.z < CHUNK_SIZE &&
			c->blocks[adj.x + CHUNK_SIZE * adj.y + CHUNK_FACE *adj.z])
			continue;

		glBindTexture(GL_TEXTURE_2D, textures[i]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices) / 6, cubeVertices + 30 * i, GL_STATIC_DRAW);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
	glUseProgram(0);
}
void drawChunk(Chunk* c, unsigned int* textures, unsigned int program, mat4s mvp, unsigned int mvpLoc, unsigned int VAOcube) {
	// TODO: get texture from block ID
	
	// translate mvp by chunk position
	vec3s chunkPos = {{ c->pos.x, c->pos.y, c->pos.z }};
	mvp = glms_translate(mvp, chunkPos);

	for (int x = 0; x < CHUNK_SIZE; x++) {
		for (int y = 0; y < CHUNK_SIZE; y++) {
			for (int z = 0; z < CHUNK_SIZE; z++) {
				// continue on air block
				if (!c->blocks[x + CHUNK_SIZE * y + CHUNK_FACE * z]) {
					mvp = glms_translate_z(mvp, 1.0f);
					continue;
				}

				ivec3 blockPos = { 0, 0, 0 };
				// add chunk position
				glm_ivec3_add((ivec3){ x, y, z }, c->pos.raw, blockPos);

				drawCube(textures, c, (ivec3s){{ x, y, z }}, program, mvp, mvpLoc, VAOcube);
				
				// translations to move across chunk
				mvp = glms_translate_z(mvp, 1.0f);
			}
			mvp = glms_translate_z(mvp, -CHUNK_SIZE);
			
			mvp = glms_translate_y(mvp, 1.0f);
		}
		mvp = glms_translate_y(mvp, -CHUNK_SIZE);
		
		mvp = glms_translate_x(mvp, 1.0f);
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
	glGenBuffers(2, VBOs);

	// cube
	unsigned int VAOcube = genBindVAO(VBOs[0], cubeVertices, sizeof(cubeVertices));
	setStdCubePointerArithmetic();

	unsigned int cubeProgram = buildShaderProgram("spin_cube_mvp.vert", "spin_cube.frag");

	// textures
	stbi_set_flip_vertically_on_load(true);
	int grassTopW, grassTopH, grassTopChannels;
	int grassSideW, grassSideH, grassSideChannels;
	int dirtW, dirtH, dirtChannels;
	char* imgGrassTop = stbi_load("images\\1_grass_top.png", &grassTopW, &grassTopH, &grassTopChannels, 0);
	char* imgGrassSide = stbi_load("images\\1_grass_side.png", &grassSideW, &grassSideH, &grassSideChannels, 0);
	char* imgDirt = stbi_load("images\\1_dirt.png", &dirtW, &dirtH, &dirtChannels, 0);
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

	// chunk example
	Chunk* c = malloc(sizeof(Chunk));
	if (!c)
		exit(0);
	c->pos = (ivec3s){ 0, 1 - CHUNK_SIZE, 0 };
	for (int x = 0; x < CHUNK_SIZE; x++)
		for (int z = 0; z < CHUNK_SIZE; z++)
			for (int y = 0; y < CHUNK_SIZE; y++)
				c->blocks[x + CHUNK_SIZE * y + CHUNK_FACE * z] = 1;

	// render loop
	while (!glfwWindowShouldClose(window)) {
		float frameDelta = updateFrameDelta(&lastFrame);
		updateFramerate(window);

		// update color
		glClearColor(0.1f, 0.4f, 0.55f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// mvp
		mat4s model = glms_translate_make((vec3s) {{ -5.0f, -1.0f, -5.0f }});
		mat4s view = glms_lookat(cam.pos, glms_vec3_add(cam.pos, cam.front), cam.up);
		mat4s perspective = glms_perspective(glm_rad(cam.fov), g_width / g_height, NEAR_PLANE, FAR_PLANE);
		mat4s mvp = glms_mul(glms_mul(perspective, view), model);

		processInput(window, frameDelta);
		drawChunk(c, grassTextures, cubeProgram, mvp, mvpLoc, VAOcube);

		//printf("%f\t%f\t%f\n", cam.front.x, cam.front.y, cam.front.z);

		glfwPollEvents();
		glfwSwapBuffers(window);
		Sleep(1);
	}
	free(c);
	glfwTerminate();
}