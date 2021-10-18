#pragma once
#include "Scene.h"
#include "tools/standardModel.h"
#include "sampler/texture.h"

class Renderer
{
public:
	Shader render_shader;
	Shader skybox_shader;
	Shader depth_shader;
	Shader debug_depth_shader;

	unsigned int SHADOW_WIDTH, SHADOW_HEIGHT;
	DepthTexture depthTexture;

	//debug
	GLuint quad_VAO, quad_VBO;

public:
	unsigned int width, height;

private:
	void renderScene(Scene& scene);
	void drawShadow(Scene& scene);

public:
	void init(Scene& scene, unsigned int width, unsigned int height);
	void render(Scene& scene);
	void endFrame();

};

