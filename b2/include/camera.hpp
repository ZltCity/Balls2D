#pragma once

#include <glm/glm.hpp>

namespace b2
{

class Camera
{
public:
	Camera() = default;
	Camera(const glm::vec3 &position, const glm::vec3 &target, const glm::vec3 &up);

	void rotate(float angle);
	void lookAt(const glm::vec3 &position, const glm::vec3 &target, const glm::vec3 &up);

	glm::vec3 getPosition() const;
	glm::mat4 getView() const;
	glm::mat4 getPerspective(float fovy, float aspect, float zfar, float znear = .5f) const;

private:
	float getXZlength();

	glm::vec3 position, target, up;
	float orbitalXZangle;
};

} // namespace b2
