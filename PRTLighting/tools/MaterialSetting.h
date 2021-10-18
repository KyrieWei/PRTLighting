#pragma once
#include <glm/glm.hpp>


class MaterialSetting
{
public:
	glm::vec3 diffuseColor, specularColor = glm::vec3(1.0f);
	float specularReflectivity, diffuseReflectivity, emissivity, specularDiffusion = 2.0f;
	float transparency = 0.0f, refractiveIndex = 1.4f;

	MaterialSetting(glm::vec3 _diffuseColor = glm::vec3(1.0f), 
					float _emissivity = 0.0f, 
					float _specularReflectivity = 0.0f,
					float _diffuseReflectivity = 1.0f) :
		diffuseColor(_diffuseColor),
		emissivity(_emissivity),
		specularReflectivity(_specularReflectivity),
		diffuseReflectivity(_diffuseReflectivity) { }
};