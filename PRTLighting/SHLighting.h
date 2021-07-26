#pragma once
#include <string>

#include "tools/Camera.h"
#include "tools/Shader.h"
#include "tools/Utility.h"
#include "sampler/harmonics.h"
#include "sampler/texture.h"

class SHLighting
{
public:
	int sh_degrees;
	int coeff_num;
	glm::vec4* sh_coeff;

public:
	SHLighting();
	~SHLighting();

	void cal_SH_coeff(unsigned int skyboxID);
	void save_sh_texture();
};

