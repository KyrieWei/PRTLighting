#include "texture.h"
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#include "../tools/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_STATIC
#include "../tools/stb_image_write.h"

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

texture::texture()
{
	textureID = 0;
}

void texture::write_to_jpg(const char* str, int width, int height, int channel, unsigned char* data)
{
	if (data == NULL)
		return;

	stbi_write_jpg(str, width, height, channel, data, 100);

}

unsigned int texture::load_single_texture(const char* str)
{

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	int width, height, channel;

	unsigned char* data = stbi_load(str, &width, &height, &channel, 0);

	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	stbi_image_free(data);

    return textureID;
}

unsigned int texture::load_cube_map(std::vector<std::string> faces)
{
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int skybox_width, skybox_height, skybox_channel;
    for (int i = 0; i < faces.size(); i++)
    {
        unsigned char* skybox_data = stbi_load(faces[i].c_str(), &skybox_width, &skybox_height, &skybox_channel, 0);
        if (skybox_data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, skybox_width, skybox_height, 0, GL_RGB, GL_UNSIGNED_BYTE, skybox_data);
            stbi_image_free(skybox_data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(skybox_data);
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

unsigned int texture::load_bmp_cube_map(std::vector<std::string> faces)
{
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int skybox_width, skybox_height, skybox_channel;
    for (int i = 0; i < faces.size(); i++)
    {
        GLubyte* skybox_data = loadBMP(faces[i], skybox_width, skybox_height);
        //unsigned char* skybox_data = stbi_load(faces[i].c_str(), &skybox_width, &skybox_height, &skybox_channel, 0);

        if (skybox_data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, skybox_width, skybox_height, 0, GL_BGR, GL_UNSIGNED_BYTE, skybox_data);
            stbi_image_free(skybox_data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(skybox_data);
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

//std::vector<int> texture::sample_texture(const texture& text, double theta, double phi)
//{
//	std::vector<int> color;
//	
//	int row = static_cast<int>(theta / PI * text.height);
//	int col = static_cast<int>(phi / (PI * 2) * text.width);
//
//	int index = (row * text.width + col) * text.channel;
//
//	if (text.channel == 3)
//	{
//		color.push_back(static_cast<int>(text.texture_data[index]));
//		color.push_back(static_cast<int>(text.texture_data[index+1]));
//		color.push_back(static_cast<int>(text.texture_data[index+2]));
//	}
//	else 
//	{
//		color.push_back(static_cast<int>(text.texture_data[index]));
//		color.push_back(static_cast<int>(text.texture_data[index + 1]));
//		color.push_back(static_cast<int>(text.texture_data[index + 2]));
//		color.push_back(static_cast<int>(text.texture_data[index + 3]));
//	}
//
//	return color;
//}
//
//std::vector<double> texture::cal_test_light(const texture& text, double theta, double phi)
//{
//	double res = std::max(0.0, 5 * cos(theta) - 4) + std::max(0.0, -4 * sin(theta - PI) * cos(phi - 2.5) - 3);
//
//	return std::vector<double>{res, res, res};
//}

void DepthTexture::init(unsigned int width, unsigned int height)
{
    SHADOW_WIDTH = width;
    SHADOW_HEIGHT = height;

    glGenFramebuffers(1, &depthMapFBO);

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, textureID, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DepthTexture::bind(unsigned int unit)
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, textureID);
}

