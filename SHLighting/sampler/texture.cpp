#include "texture.h"
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#include "../tools/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_STATIC
#include "../tools/stb_image_write.h"

#include <iostream>

texture::texture()
{
	width = 0;
	height = 0;
	channel = 0;
	texture_data = NULL;
}

texture::texture(const char* str)
{
	if (texture_data != NULL)
	{
		delete[] texture_data;
		texture_data = NULL;
	}

	texture_data = stbi_load(str, &width, &height, &channel, 0);

	if (texture_data == NULL)
	{
		std::cout << "failed to load texture " << str << std::endl;
	}
}

texture::~texture()
{
	if (texture_data != NULL)
	{
		delete[] texture_data;
		texture_data = NULL;
	}
}

void texture::write_to_jpg(const char* str, int width, int height, int channel, unsigned char* data)
{
	if (data == NULL)
		return;

	stbi_write_jpg(str, width, height, channel, data, 100);

}

std::vector<int> texture::sample_texture(const texture& text, double theta, double phi)
{
	std::vector<int> color;
	
	int row = static_cast<int>(theta / PI * text.height);
	int col = static_cast<int>(phi / (PI * 2) * text.width);

	int index = (row * text.width + col) * text.channel;

	if (text.channel == 3)
	{
		color.push_back(static_cast<int>(text.texture_data[index]));
		color.push_back(static_cast<int>(text.texture_data[index+1]));
		color.push_back(static_cast<int>(text.texture_data[index+2]));
	}
	else 
	{
		color.push_back(static_cast<int>(text.texture_data[index]));
		color.push_back(static_cast<int>(text.texture_data[index + 1]));
		color.push_back(static_cast<int>(text.texture_data[index + 2]));
		color.push_back(static_cast<int>(text.texture_data[index + 3]));
	}

	return color;
}

std::vector<double> texture::cal_test_light(const texture& text, double theta, double phi)
{
	double res = std::max(0.0, 5 * cos(theta) - 4) + std::max(0.0, -4 * sin(theta - PI) * cos(phi - 2.5) - 3);

	return std::vector<double>{res, res, res};
}



