// Std. Includes
#include <vector>
#include <iostream>

// GL Includes
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <camera.h>


Camera::Camera(glm::vec3 position, glm::vec3 up,
			   GLfloat yaw, GLfloat pitch)
:front(glm::vec3(0.0f, 0.0f, -1.0f)),
 movementSpeed(SPEED),
 mouseSensitivity(SENSITIVTY),
 zoom(ZOOM)
{

	this->position = position;
	this->worldUp = up;
	this->yaw = yaw;
	this->pitch = pitch;

	this->updateCameraVectors();
}



glm::mat4
Camera::getViewMatrix()
{
	return glm::lookAt(position, /* Location */
					   position + front, /* Target */
					   up /* Up */);
}


glm::mat4
Camera::getProjectionMatrix()
{
	return glm::perspective(zoom,
			(GLfloat)800 / (GLfloat) 600, 0.1f, 100.0f);
}



void
Camera::processKeyboardMovement(Camera_Movement direction, GLfloat deltaTime, GLfloat doubleTime)
{
	GLfloat velocity = movementSpeed * deltaTime * doubleTime;

	if (direction == FORWARD)
		position += front * velocity;
	else if (direction == BACKWARD)
		position -= front * velocity;
	else if (direction == LEFT)
		position -= right * velocity;
	else if (direction == RIGHT)
		position += right * velocity;
	else if (direction == UP)
		position += up * velocity;
	else if (direction == DOWN)
		position -= up * velocity;
}


void
Camera::processMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch)
{

	GLfloat xOffset = xoffset * mouseSensitivity;
	GLfloat yOffset = yoffset * mouseSensitivity;

	yaw += xOffset;
	pitch += yOffset;

	// Make sure that when pitch is out of bounds, screen doesn't get flipped
	if (constrainPitch)
	{
		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;
	}

	// Update Front, Right and Up Vectors using the updated Eular angles
	this->updateCameraVectors();
}

// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
void
Camera::processZoom(GLfloat yoffset)
{
	if (zoom >= 43.0f && zoom <= 47.0f)
		zoom -= yoffset;

	else if (zoom <= 43.0f)
		zoom = 43.0f;
	else if (zoom >= 47.0f)
		zoom = 47.0f;


}


void
Camera::updateCameraVectors()
{
	// Calculate the new Front vector
	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

	this->front = glm::normalize(front);
	// Also re-calculate the Right and Up vector
	this->right = glm::normalize(glm::cross(front, worldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	this->up    = glm::normalize(glm::cross(right, front));
}
