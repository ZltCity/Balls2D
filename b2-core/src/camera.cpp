#include <glm/gtc/matrix_transform.hpp>

#include "camera.hpp"

namespace b2
{

Camera::Camera(const glm::vec3 &position, const glm::vec3 &target, const glm::vec3 &up) : orbitalXZangle(0.0f)
{
	lookAt(position, target, up);
}

void Camera::rotate(float angle)
{
	float length = getXZlength();

	orbitalXZangle += angle;

	position = glm::vec3(
		glm::sin(glm::radians(orbitalXZangle)) * length, position.y, glm::cos(glm::radians(orbitalXZangle)) * length);
}

void Camera::lookAt(const glm::vec3 &position, const glm::vec3 &target, const glm::vec3 &up)
{
	this->position = position;
	this->target = target;
	this->up = up;

	orbitalXZangle = glm::degrees(glm::asin(position.x / getXZlength()));
}

glm::vec3 Camera::getPosition() const
{
	return position;
}

glm::mat4 Camera::getView() const
{
	return glm::lookAtLH(position, target, up);
}

glm::mat4 Camera::getPerspective(float fovy, float aspect, float zfar, float znear) const
{
	return glm::perspectiveLH(glm::radians(fovy), aspect, znear, zfar);
}

float Camera::getXZlength()
{
	return glm::length(glm::vec2(position.x, position.z) - glm::vec2(target.x, target.z));
}

} // namespace b2-core
