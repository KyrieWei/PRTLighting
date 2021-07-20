#pragma once
#include <string>
#include <vector>

#include "../tools/Maths.h"

class texture
{
public:
	int width, height, channel;
	std::string texture_name;

	unsigned char* texture_data;

public:
	texture();
	texture(const char* str);
	~texture();

	void write_to_jpg(const char* str, int width, int height, int channel, unsigned char* data);

	static std::vector<int> sample_texture(const texture& text, double theta, double phi);
	static std::vector<double> cal_test_light(const texture& text, double theta, double phi);
};

