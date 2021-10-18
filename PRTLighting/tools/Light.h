#pragma once
#include "Shader.h"

class Light
{
};

class DirectionalLight
{
public:
	glm::vec3 direction, radiance;
	DirectionalLight(){}

	void init(glm::vec3 _direction, glm::vec3 _radiance)
	{
		direction = _direction;
		radiance = _radiance;
	}

	void Upload(Shader& shader) const
	{
		shader.setVec3("directionalLight.direction", direction);
		shader.setVec3("directionalLight.radiance", radiance);
	}
};

