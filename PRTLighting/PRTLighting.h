#pragma once
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "tools/Camera.h"
#include "tools/Shader.h"
#include "tools/Utility.h"
#include "tools/Singleton.h"
#include "SHLighting.h"
#include "SimpleScene.h"
#include "Renderer.h"

class PRTLighting : public Singleton<PRTLighting>
{
public:
	unsigned int width, height;
	std::string title;
	
	SimpleScene simpleScene;
	Renderer renderer;
	GLFWwindow* window;

	typedef std::shared_ptr<PRTLighting> ptr;

	static float deltaTime, lastFrame;
	static float lastX, lastY;
	static bool firstMouse;
	
public:
	PRTLighting() {}
	void setWindowSize(unsigned int width_, unsigned int height_);
	void setWindowTitle(const std::string& title);
	void init();
	void run();

	static PRTLighting::ptr getSingleton();

protected:
	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	static void processInput(GLFWwindow* window, float deltaTime);

};

