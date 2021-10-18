#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/quaternion.hpp>

class Transform
{
public:
	glm::vec3 position = { 0, 0, 0 };
	glm::vec3 scale = { 1, 1, 1 };
	glm::vec3 rotation = { 0, 0, 0 };

	bool transformIsInvalid = false;

	Transform();

	void updateTransformMatrix();

	glm::mat4& getTransformMatrix();

private:
	glm::mat4 transform;

};

