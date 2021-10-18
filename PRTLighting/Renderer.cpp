#include "Renderer.h"

void Renderer::init(Scene& scene, unsigned int width, unsigned int height)
{
	render_shader = Shader("PRTLighting/shaders/render_vert.vs", "PRTLighting/shaders/render_frag.fs");
	skybox_shader = Shader("PRTLighting/shaders/skybox_vert.vs", "PRTLighting/shaders/skybox_frag.fs");
	depth_shader = Shader("PRTLighting/shaders/depth_vert.vs", "PRTLighting/shaders/depth_frag.fs");
	debug_depth_shader = Shader("PRTLighting/shaders/debug_depth_vert.vs", "PRTLighting/shaders/debug_depth_frag.fs");

	this->width = width;
	this->height = height;

	SHADOW_WIDTH = 2048;
	SHADOW_HEIGHT = 2048;
	depthTexture.init(SHADOW_WIDTH, SHADOW_HEIGHT);

	//----------------------------debug depth-----------------------------
	float quad_vertex[] =
	{
		// positions        // texture Coords
		-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
		 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	};

	glGenVertexArrays(1, &quad_VAO);
	glGenBuffers(1, &quad_VBO);

	glBindVertexArray(quad_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, quad_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertex), quad_vertex, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
	//---------------------------------------------------------------------
}

void Renderer::render(Scene& scene)
{
	//draw shadow
	drawShadow(scene);

	renderScene(scene);
}

void Renderer::drawShadow(Scene& scene)
{

	glEnable(GL_DEPTH_TEST);

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthTexture.depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	scene.uploadShadowInfo(depth_shader);

	scene.draw(depth_shader);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//debug depth
	//glViewport(0, 0, width, height);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//debug_depth_shader.use();
	//debug_depth_shader.setFloat("near_plane", 1.0f);
	//debug_depth_shader.setFloat("far_plane", 3000.0f);
	//debug_depth_shader.setInt("depthMap", 0);
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, depthTexture.textureID);

	//glBindVertexArray(quad_VAO);
	//glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	//glBindVertexArray(0);
}

void Renderer::renderScene(Scene& scene)
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glViewport(0, 0, width, height);

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	scene.uploadCameraInfo(render_shader);
	scene.uploadLightInfo(render_shader);
	
	//bind depth texture
	scene.uploadShadowInfo(render_shader);
	render_shader.setInt("texture_depth", 4);
	depthTexture.bind(4);

	//opaque
	scene.draw(render_shader);

	//skybox
	glDepthFunc(GL_LEQUAL);
	glDisable(GL_CULL_FACE);
	scene.uploadCameraInfo(skybox_shader);
	scene.draw_skybox(skybox_shader);
}


void Renderer::endFrame()
{


}

