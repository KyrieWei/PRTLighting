#pragma once
#include "tools/Model.h"
#include "tools/Camera.h"

class Scene
{
public:
	Camera camera;
	std::vector<Model> models;
	glm::vec3 bb_min, bb_max;
	
	virtual void uploadCameraInfo(Shader& shader) = 0;
	virtual void uploadLightInfo(Shader& shader) = 0;
	virtual void uploadShadowInfo(Shader& shader) = 0;
	virtual void init(int viewportwidth, int viewportheight) = 0;
	virtual void draw(Shader& shader) = 0;
	virtual void draw_skybox(Shader& shader) = 0;
};