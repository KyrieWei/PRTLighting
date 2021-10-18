#pragma once
#include "Scene.h"
#include "tools/standardModel.h"
#include "tools/Light.h"

class SimpleScene : public Scene
{
private:
	glm::vec3 dirLightPos;

public:
	
	SkyBox skybox;
	DirectionalLight directionalLight;

public:
	SimpleScene() {}

	virtual void uploadCameraInfo(Shader& shader);
	virtual void uploadLightInfo(Shader& shader);
	virtual void uploadShadowInfo(Shader& shader);
	virtual void init(int viewportwidth, int viewportheight);
	virtual void draw(Shader& shader);
	virtual void draw_skybox(Shader& shader);
};

