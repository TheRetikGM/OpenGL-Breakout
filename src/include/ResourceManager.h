#pragma once
#include <map>
#include <string>
#include "CustomTypes.h"
#include "Texture.h"
#include "Shader.h"

class ResourceManager
{
public:
	static std::map<std::string, Shader>	Shaders;
	static std::map<std::string, Texture2D> Textures;

	static Shader LoadShader(c_string vShaderFile, c_string fShaderFile, c_string gShaderFile, std::string name);
	static Shader GetShader(std::string name);
	static Texture2D LoadTexture(c_string file, bool alpha, std::string name);
	static Texture2D GetTexture(std::string name);
	static void Clear();
private:
	ResourceManager() { }
	static Shader loadShaderFromFile(c_string vShaderFile, c_string fShaderFile, c_string gShaderFile = nullptr);
	static Texture2D loadTextureFromFile(c_string file, bool alpha);
};