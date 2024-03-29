#pragma once
#include <cglm/struct.h>

///////////////////////////////////////////////////////////////////////////////
// constants
#define JUMP_VELOCITY 0.18f
#define DRAG 0.98f
#define GRAVITY 0.01f
#define NEAR_PLANE 0.1f
#define FAR_PLANE 100.0f
#define FOV_MIN 30.0f
#define FOV_MAX 150.0f

// change to #define FLY_CAM to enable free flying movement
#define FLY_CAM_

enum Side {
	FRONT,
	BACK,
	LEFT,
	RIGHT,
	DOWN,
	UP
};

///////////////////////////////////////////////////////////////////////////////
// camera

typedef struct {
	// cam attributes
	vec3s pos;
	vec3s front;
	vec3s up;
	vec3s right;
	vec3s worldUp;
	float yVelocity;
	// euler angles
	float yaw;
	float pitch;
	// settings
	float speed;
	float sens;
	float fov;
	// mouse memory
	float lastMouseX;
	float lastMouseY;
	bool firstMouse;
} Camera;

///////////////////////////////////////////////////////////////////////////////
// functions

void cam_updatePos(Camera* cam, GLFWwindow* window, float frameDelta);
void cam_updateVectors(Camera* cam);
vec3s cam_moveForward(Camera* cam);
vec3s cam_moveBackward(Camera* cam);
vec3s cam_moveRight(Camera* cam);
vec3s cam_moveLeft(Camera* cam);
bool cam_isOnFloor(Camera* cam);
void cam_jump(Camera* cam);