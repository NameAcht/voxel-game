#include <glfw3.h>
#include <cglm/struct.h>
#include <mc_gl.h>
#include <camera.h>

// fps style cam
void cam_updatePos(Camera* cam, GLFWwindow* window, float frameDelta) {
	cam->speed = 2.5f * frameDelta;
	
	vec3s moveVec = {{ 0.0f, 0.0f, 0.0f }};
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT))
		cam->speed *= 1.75f;
	if (glfwGetKey(window, GLFW_KEY_W))
		moveVec = glms_vec3_add(moveVec, cam_moveForward(cam));
	if (glfwGetKey(window, GLFW_KEY_S))
		moveVec = glms_vec3_add(moveVec, cam_moveBackward(cam));
	if (glfwGetKey(window, GLFW_KEY_D))
		moveVec = glms_vec3_add(moveVec, cam_moveRight(cam));
	if (glfwGetKey(window, GLFW_KEY_A))
		moveVec = glms_vec3_add(moveVec, cam_moveLeft(cam));
	moveVec = glms_normalize(moveVec);

	#ifndef FLY_CAM
	cam->pos = glms_vec3_add(cam->pos, glms_vec3_scale(moveVec, cam->speed));
	// y movement
	cam->pos.y += cam->yVelocity;
	cam->yVelocity = (cam->yVelocity - GRAVITY) * DRAG;
	if (cam_isOnFloor(cam)) {
		cam->pos.y = 1.0f;
		cam->yVelocity = 0.0f;
	}
	#else
	cam->pos = glms_vec3_add(cam->pos, glms_vec3_scale(moveVec, cam->speed));
	#endif
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// cam utils

vec3s cam_moveForward(Camera* cam) {
	#ifndef FLY_CAM
	vec3s toScale = {{ cam->front.x, 0.0f, cam->front.z }};
	#else
	vec3s toScale = { cam->front.x, cam->front.y, cam->front.z };
	#endif
	toScale = glms_vec3_normalize(toScale);
	return toScale;
}
vec3s cam_moveBackward(Camera* cam) {
	vec3s toScale = {{ -cam->front.x, 0.0f, -cam->front.z }};
	toScale = glms_vec3_normalize(toScale);
	return toScale;
}
vec3s cam_moveRight(Camera* cam) {
	vec3s toScale = {{ cam->right.x, 0.0f, cam->right.z }};
	toScale = glms_vec3_normalize(toScale);
	return toScale;
}
vec3s cam_moveLeft(Camera* cam) {
	vec3s toScale = {{ -cam->right.x, 0.0f, -cam->right.z }};
	toScale = glms_vec3_normalize(toScale);
	return toScale;
}
void cam_updateVectors(Camera* cam) {
	float xRota = glm_rad(cam->pitch);
	float yRota = glm_rad(cam->yaw);
	vec3s direction = {
		.x = cos(yRota) * cos(xRota),
		.y = sin(xRota),
		.z = sin(yRota) * cos(xRota)
	};
	cam->front = glms_normalize(direction);
	cam->right = glms_normalize(glms_cross(cam->front, cam->worldUp));
}
bool cam_isOnFloor(Camera* cam) {
	if (cam->pos.y <= 1.0f)
		return true;
	return false;
}
void cam_jump(Camera* cam) {
	if(cam_isOnFloor(cam))
		cam->yVelocity = JUMP_VELOCITY;
}