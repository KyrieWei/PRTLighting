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

