#pragma once
#include <string>
#include <vector>

#include "../tools/Maths.h"
#include "../tools/Utility.h"

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

