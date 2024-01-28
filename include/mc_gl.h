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
unsigned int genBindStdTexture(char* imgData, int width, int height);
void mc_gl();

// cube vertices
static float cubeVertices[] = {
    // front
    -0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  1.0f, 0.0f,

    // back
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    // left
    -0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 1.0f,

    // right
     0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f, 1.0f,

    // down
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    // up
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};
static float cubeVertexCount = 36;

// xzplane
static float xzPlaneVertices[] = {
    // positions         // colors
   -100.0f, -0.5f, -100.0f,   0.3f, 0.35f, 0.1f,   // top right
   -100.0f, -0.5f,  100.0f,   0.2f, 0.35f, 0.1f,   // bottom right
    100.0f, -0.5f, -100.0f,   0.2f, 0.35f, 0.1f,   // bottom left

    100.0f, -0.5f,  100.0f,   0.3f, 0.35f, 0.1f,   // top left 
   -100.0f, -0.5f,  100.0f,   0.2f, 0.35f, 0.1f,   // bottom right
    100.0f, -0.5f, -100.0f,   0.2f, 0.35f, 0.1f    // bottom left
};
// xzplane
static float xzPlaneTexVertices[] = {
    // positions              // colors          // texture
   -500.0f, -0.5f, -500.0f,   0.0f, 0.0f, 0.0f,  101.0f, -100.0f,   // top right
   -500.0f, -0.5f,  500.0f,   0.0f, 0.0f, 0.0f,  101.0f,  101.0f,   // bottom right
    500.0f, -0.5f, -500.0f,   0.0f, 0.0f, 0.0f, -100.0f, -100.0f,   // bottom left

    500.0f, -0.5f,  500.0f,   0.f, 0.0f, 0.0f, -100.0f,  101.0f,   // top left 
   -500.0f, -0.5f,  500.0f,   0.f, 0.0f, 0.0f,  101.0f,  101.0f,   // bottom right
    500.0f, -0.5f, -500.0f,   0.f, 0.0f, 0.0f, -100.0f, -100.0f    // bottom left
};