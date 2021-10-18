#pragma once
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include "Shader.h"
#include "MaterialSetting.h"

struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	glm::vec3 Tangent;
	glm::vec3 Bitangent;

	Vertex() : Tangent(glm::vec3(1.0f)), Bitangent(glm::vec3(1.0f)) {}
};

struct Texture
{
	unsigned int id;
	std::string type;
	std::string path;
};

class Mesh
{
public:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	unsigned int VAO;

	bool useMaterialSetting = false;
	MaterialSetting materialSetting;

	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
	{
		this->vertices = vertices;
		this->indices = indices;
		this->textures = textures;

		setupMesh();
	}

	~Mesh()
	{

	}

	void Draw(Shader& shader)
	{

		for (unsigned int i = 0; i < textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);

			std::string number;

			std::string name = textures[i].type;

			if (name == "texture_diffuse")
				shader.setInt("texture_diffuse", i);
			else if (name == "texture_normal")
				shader.setInt("texture_normal", i);
			else if (name == "texture_metallic")
				shader.setInt("texture_metallic", i);
			else if (name == "texture_roughness", i)
				shader.setInt("texture_roughness", i);

			glBindTexture(GL_TEXTURE_2D, textures[i].id);
		}

		//Voxel Cone Tracing 
		if (useMaterialSetting)
		{
			shader.use();
			shader.setVec3("material.diffuseColor", materialSetting.diffuseColor);
			shader.setVec3("material.specularColor", materialSetting.specularColor);
			shader.setFloat("material.specularDiffusion", materialSetting.specularDiffusion);
			shader.setFloat("material.specularReflectivity", materialSetting.specularReflectivity);
			shader.setFloat("material.diffuseReflectivity", materialSetting.diffuseReflectivity);
			shader.setFloat("material.emissivity", materialSetting.emissivity);
			shader.setFloat("material.refractiveIndex", materialSetting.refractiveIndex);
			shader.setFloat("material.transparency", materialSetting.transparency);
			
		}

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glActiveTexture(GL_TEXTURE0);
	}

private:
	unsigned int VBO, EBO;

	void setupMesh()
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

		glBindVertexArray(0);
	}

};


