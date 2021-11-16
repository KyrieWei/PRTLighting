#pragma once
#include <string>
#include <vector>

#include "../tools/Maths.h"
#include "../tools/Utility.h"
#include "../tools/Shader.h"

class texture
{
public:
	unsigned int textureID;

public:
	texture();

	void write_to_jpg(const char* str, int width, int height, int channel, unsigned char* data);

	unsigned int load_single_texture(const char* str);
	unsigned int load_cube_map(std::vector<std::string> faces);
	unsigned int load_bmp_cube_map(std::vector<std::string> faces);

	//static std::vector<int> sample_texture(const texture& text, double theta, double phi);
	//static std::vector<double> cal_test_light(const texture& text, double theta, double phi);
};

class DepthTexture
{
public:
	unsigned int textureID, depthMapFBO;
	unsigned int SHADOW_WIDTH, SHADOW_HEIGHT;

public:
	void init(unsigned int width, unsigned int height);
	void bind(unsigned int unit);
};

class Texture3D
{
public:
	unsigned char* textureBuffer = nullptr;
	GLuint textureID;

	void Activate(Shader& shader, const std::string& glSamplerName, const int textureUnit = GL_TEXTURE0);

	void Clear(GLfloat clearColor[4]);

	Texture3D(const std::vector<GLfloat>& textureBuffer, const int width, const int height, const int depth, const bool generateMipmaps = true);
	~Texture3D();
private:
	int width, height, depth;
	std::vector<GLfloat> clearData;
};
