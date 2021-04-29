#include "ResourceManager.h"
#include <exception>
#include <sstream>
#include <fstream>
#include <stb_image.h>

std::map<std::string, Shader> ResourceManager::Shaders;
std::map<std::string, Texture2D> ResourceManager::Textures;
Shader ResourceManager::LoadShader(c_string vShaderFile, c_string fShaderFile, c_string gShaderFile, std::string name)
{
	Shaders[name] = loadShaderFromFile(vShaderFile, fShaderFile, gShaderFile);
	return Shaders[name];
}
Shader ResourceManager::GetShader(std::string name)
{
	return Shaders[name];
}
Texture2D ResourceManager::LoadTexture(c_string file, bool alpha, std::string name)
{
	Textures[name] = loadTextureFromFile(file, alpha);
	return Textures[name];
}
Texture2D ResourceManager::GetTexture(std::string name)
{
	return Textures[name];
}
void ResourceManager::Clear()
{
	for (auto& i : Shaders)
		glDeleteProgram(i.second.ID);
	for (auto& i : Textures)
		glDeleteTextures(1, &i.second.ID);
}
Shader ResourceManager::loadShaderFromFile(c_string vShaderFile, c_string fShaderFile, c_string gShaderFile)
{
	std::string vertexCode;
	std::string fragmentCode;
	std::string geometryCode;
	try
	{
		std::ifstream vertexShaderFile(vShaderFile);
		std::ifstream fragmentShaderFile(fShaderFile);
		std::stringstream vShaderStream, fShaderStream;

		vShaderStream << vertexShaderFile.rdbuf();
		fShaderStream << fragmentShaderFile.rdbuf();

		vertexShaderFile.close();
		fragmentShaderFile.close();

		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();

		if (gShaderFile != nullptr)
		{
			std::ifstream geometryShaderFile(gShaderFile);
			std::stringstream gShaderStream;
			gShaderStream << geometryShaderFile.rdbuf();
			geometryShaderFile.close();
			geometryCode = gShaderStream.str();
		}
	}
	catch (std::exception& e)
	{
		throw std::exception(("ERROR::LoadShader: Failed to read shader files\n" + std::string(e.what())).c_str());
	}

	c_string vShaderCode = vertexCode.c_str();
	c_string fShaderCode = fragmentCode.c_str();
	c_string gShaderCode = geometryCode.c_str();

	Shader shader;
	shader.Compile(vShaderCode, fShaderCode, gShaderFile != nullptr ? gShaderCode : nullptr);
	return shader;
}
Texture2D ResourceManager::loadTextureFromFile(c_string file, bool alpha)
{
	Texture2D texture;
	if (alpha)
	{
		texture.Internal_format = GL_RGBA;
		texture.Image_format = GL_RGBA;
	}
	int width, height, nrChannels;
	unsigned char* data = stbi_load(file, &width, &height, &nrChannels, 0);
	texture.Generate(width, height, data);
	stbi_image_free(data);
	return texture;
}