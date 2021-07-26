#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "SHLighting.h"

SHLighting::SHLighting()
{
    sh_degrees = 3;
    coeff_num = sh_degrees * sh_degrees;

    sh_coeff = new glm::vec4[coeff_num];
}

SHLighting::~SHLighting()
{
    if (sh_coeff != NULL)
        delete[] sh_coeff;
}

void SHLighting::cal_SH_coeff(unsigned int skyboxID)
{

    int sample_size_x = 512, sample_size_y = 512;
    int thread_x = 8, thread_y = 8;
    int group_x = sample_size_x / thread_x;
    int group_y = sample_size_y / thread_y;

    Shader SHCoeffShader("PRTLighting/shaders/SHCoeff_comp.cs");

    SHCoeffShader.use();
    SHCoeffShader.setVec2("SampleSize", glm::vec2(sample_size_x, sample_size_y));

    //暂存每个线程组的sh_coeff
    glm::vec4* sh_coeff_tmp_buffer = new glm::vec4[group_x * group_y * coeff_num];

    for (int i = 0; i < coeff_num; i++)
        sh_coeff[i] = glm::vec4(0);

    //传递天空盒贴图
    SHCoeffShader.setInt("skybox", 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxID);

    //计算sh_coeff
    unsigned int shcoeff_ssbo;
    glGenBuffers(1, &shcoeff_ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, shcoeff_ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4) * group_x * group_y * coeff_num, NULL , GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, shcoeff_ssbo);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    unsigned int shcoeff_blockIndex = glGetProgramResourceIndex(SHCoeffShader.ID, GL_SHADER_STORAGE_BLOCK, "sh_coeff_buffer");

    if (shcoeff_blockIndex != GL_INVALID_INDEX)
    {
        glShaderStorageBlockBinding(SHCoeffShader.ID, shcoeff_blockIndex, 0);
    }
    else
    {
        std::cout << "binding sh_coeff_buffer not found" << std::endl;
    }
    
    glDispatchCompute((GLuint)group_x, (GLuint)group_y, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    
    //将compute shader中的sh_coeff_buffer数据取出来
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, shcoeff_ssbo);
    void* ptr = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
    memcpy(sh_coeff_tmp_buffer, ptr, sizeof(glm::vec4) * group_x * group_y * coeff_num);
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

    //将每个线程组的sh_coeff相加得到最终的sh_coeff
    for (int i = 0; i < group_x * group_y; i++)
        for(int j = 0; j < coeff_num; j++)
            sh_coeff[j] += sh_coeff_tmp_buffer[i * coeff_num + j];

    //print sh_coeff
    for (int i = 0; i < coeff_num; i++)
        std::cout << sh_coeff[i].x << " " << sh_coeff[i].y << " " << sh_coeff[i].z << " " << sh_coeff[i].w << std::endl;

    delete[] sh_coeff_tmp_buffer;

}
