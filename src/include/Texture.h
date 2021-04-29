#pragma once
#include <glad/glad.h>
#include "CustomTypes.h"

class Texture2D
{
public:
	uint ID;
	uint Width, Height;
	uint Internal_format;
	uint Image_format;

	uint Wrap_S, Wrap_T;
	uint Filter_min, Filter_mag;

	Texture2D();	

	void Generate(uint width, uint height, unsigned char* data);

	void Bind() const;
};