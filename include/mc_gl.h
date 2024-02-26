#pragma once
#include <camera.h>
#include <glfw3.h>

#define X_AXIS (vec3s){ 1.0f, 0.0f, 0.0f }
#define Y_AXIS (vec3s){ 0.0f, 1.0f, 0.0f }
#define Z_AXIS (vec3s){ 0.0f, 0.0f, 1.0f }

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 900
#define CENTER_OFFSET 0.2f

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
GLFWwindow* initWindow(int width, int height);
unsigned int genBindVAO(unsigned int VBO, float vertices[], size_t size);
unsigned int genBindVBO(float vertices[], size_t size);
unsigned int genBindEBO(float vertices[], unsigned int indices[], size_t iSize);
void textureRectanglePointerArithmetic();
unsigned int genBindStdTexture(char* imgData, int width, int height, int nrChannels);
void mc_gl();

// cube vertices
static float cubeVertices[] = {
    // front
     0.0f,  0.0f,  0.0f,  1.0f, 0.0f,
     1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
     1.0f,  1.0f,  0.0f,  0.0f, 1.0f,
     1.0f,  1.0f,  0.0f,  0.0f, 1.0f,
     0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
     0.0f,  0.0f,  0.0f,  1.0f, 0.0f,

    // back
     0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
     1.0f,  0.0f,  1.0f,  1.0f, 0.0f,
     1.0f,  1.0f,  1.0f,  1.0f, 1.0f,
     1.0f,  1.0f,  1.0f,  1.0f, 1.0f,
     0.0f,  1.0f,  1.0f,  0.0f, 1.0f,
     0.0f,  0.0f,  1.0f,  0.0f, 0.0f,

    // left
     0.0f,  1.0f,  1.0f,  1.0f, 1.0f,
     0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
     0.0f,  0.0f,  0.0f,  0.0f, 0.0f,
     0.0f,  0.0f,  0.0f,  0.0f, 0.0f,
     0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
     0.0f,  1.0f,  1.0f,  1.0f, 1.0f,

    // right
     1.0f,  1.0f,  1.0f,  0.0f, 1.0f,
     1.0f,  1.0f,  0.0f,  1.0f, 1.0f,
     1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
     1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
     1.0f,  0.0f,  1.0f,  0.0f, 0.0f,
     1.0f,  1.0f,  1.0f,  0.0f, 1.0f,

    // down
     0.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
     1.0f,  0.0f,  1.0f,  1.0f, 0.0f,
     1.0f,  0.0f,  1.0f,  1.0f, 0.0f,
     0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
     0.0f,  0.0f,  0.0f,  0.0f, 1.0f,

    // up
     0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
     1.0f,  1.0f,  0.0f,  1.0f, 1.0f,
     1.0f,  1.0f,  1.0f,  1.0f, 0.0f,
     1.0f,  1.0f,  1.0f,  1.0f, 0.0f,
     0.0f,  1.0f,  1.0f,  0.0f, 0.0f,
     0.0f,  1.0f,  0.0f,  0.0f, 1.0f
};
static float cubeVertexCount = 36;