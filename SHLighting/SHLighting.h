#pragma once
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "tools/Camera.h"
#include "tools/Shader.h"
#include "tools/Utility.h"
#include "sampler/harmonics.h"
#include "sampler/texture.h"

class SHLighting
{
public:
	unsigned int width, height;
	std::string title;

	Camera camera;
	float lastX;
	float lastY;
	bool firstMouse;

	//time
	float deltaTime;
	float lastFrame;

	//harmonics m_harmonics;

public:
	SHLighting();
	void setWindowSize(unsigned int width_, unsigned int height_);
	void setWindowTitle(const std::string& title);
	void run();

	void save_sh_texture();

	void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	void processInput(GLFWwindow* window, float deltaTime);
};

