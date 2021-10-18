#include "Transform.h"

Transform::Transform()
{
	updateTransformMatrix();
}

void Transform::updateTransformMatrix()
{
	transform = glm::translate(position) * glm::mat4_cast(glm::quat(rotation)) * glm::scale(scale);
	transformIsInvalid = false;
}

glm::mat4& Transform::getTransformMatrix()
{
	if (transformIsInvalid)
		updateTransformMatrix();

	return transform;
}

