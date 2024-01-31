#include <glfw3.h>
#include <cglm/struct.h>
#include <mc_gl.h>
#include <camera.h>

// fps style cam
void cam_updatePos(Camera* cam, GLFWwindow* window, float frameDelta) {
	cam->speed = 2.5f * frameDelta;
	
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT))
		cam->speed *= 1.75f;
	if (glfwGetKey(window, GLFW_KEY_W))
		cam_moveForward(cam);
	if (glfwGetKey(window, GLFW_KEY_S))
		cam_moveBackward(cam);
	if (glfwGetKey(window, GLFW_KEY_D))
		cam_moveRight(cam);
	if (glfwGetKey(window, GLFW_KEY_A))
		cam_moveLeft(cam);

	// y movement
	cam->pos.y += cam->yVelocity;
	cam->yVelocity = (cam->yVelocity - GRAVITY) * 0.98f;
	if (cam_isOnFloor(cam)) {
		cam->pos.y = 1.0f;
		cam->yVelocity = 0.0f;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// cam utils

void cam_moveForwardFly(Camera* cam) {
	vec3s toScale = { cam->front.x, cam->front.y, cam->front.z };
	toScale = glms_vec3_normalize(toScale);
	cam->pos = glms_vec3_add(cam->pos, glms_vec3_scale(toScale, cam->speed));
}
void cam_moveForward(Camera* cam) {
	vec3s toScale = { cam->front.x, 0.0f, cam->front.z };
	toScale = glms_vec3_normalize(toScale);
	cam->pos = glms_vec3_add(cam->pos, glms_vec3_scale(toScale, cam->speed));
}
void cam_moveBackward(Camera* cam) {
	vec3s toScale = { cam->front.x, 0.0f, cam->front.z };
	toScale = glms_vec3_normalize(toScale);
	cam->pos = glms_vec3_sub(cam->pos, glms_vec3_scale(toScale, cam->speed));
}
void cam_moveRight(Camera* cam) {
	vec3s toScale = { cam->right.x, 0.0f, cam->right.z };
	toScale = glms_vec3_normalize(toScale);
	cam->pos = glms_vec3_add(cam->pos, glms_vec3_scale(toScale, cam->speed));
}
void cam_moveLeft(Camera* cam) {
	vec3s toScale = { cam->right.x, 0.0f, cam->right.z };
	toScale = glms_vec3_normalize(toScale);
	cam->pos = glms_vec3_sub(cam->pos, glms_vec3_scale(toScale, cam->speed));
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