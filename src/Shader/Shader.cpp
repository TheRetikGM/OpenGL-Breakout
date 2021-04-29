#include "Shader.h"

Shader& Shader::Use()
{
	glUseProgram(ID);
	return *this;
}

void Shader::Compile(c_string vertexSource, c_string fragmentSource, c_string geometrySource)
{
	uint sVertex, sFragment, sGeom;

	sVertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(sVertex, 1, &vertexSource, NULL);
	glCompileShader(sVertex);
	checkCompileErrors(sVertex, "VERTEX");

	sFragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(sFragment, 1, &fragmentSource, NULL);
	glCompileShader(sFragment);
	checkCompileErrors(sFragment, "FRAGMENT");

	if (geometrySource != nullptr)
	{
		sGeom = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(sGeom, 1, &geometrySource, NULL);
		glCompileShader(sGeom);
		checkCompileErrors(sGeom, "GEOMETRY");
	}

	this->ID = glCreateProgram();
	glAttachShader(this->ID, sVertex);
	glAttachShader(this->ID, sFragment);
	if (geometrySource != nullptr)
		glAttachShader(this->ID, sGeom);
	glLinkProgram(this->ID);
	checkCompileErrors(this->ID, "PROGRAM");

	glDeleteShader(sVertex);
	glDeleteShader(sFragment);
	if (geometrySource != nullptr)
		glDeleteShader(sGeom);
}

void Shader::SetFloat(c_string name, float value, bool useShader)
{
	if (useShader)
		this->Use();
	glUniform1f(glGetUniformLocation(this->ID, name), value);
}
void Shader::SetInt(c_string name, int value, bool useShader)
{
	if (useShader)
		this->Use();
	glUniform1i(glGetUniformLocation(this->ID, name), value);
}
void Shader::SetVec2f(c_string name, float x, float y, bool useShader)
{
	if (useShader)
		this->Use();
	glUniform2f(glGetUniformLocation(this->ID, name), x, y);
}
void Shader::SetVec2f(c_string name, const glm::vec2& value, bool useShader)
{
	if (useShader)
		this->Use();
	glUniform2f(glGetUniformLocation(this->ID, name), value.x, value.y);
}
void Shader::SetVec3f(c_string name, float x, float y, float z, bool useShader)
{
	if (useShader)
		this->Use();
	glUniform3f(glGetUniformLocation(this->ID, name), x, y, z);
}
void Shader::SetVec3f(c_string name, const glm::vec3& value, bool useShader)
{
	if (useShader)
		this->Use();
	glUniform3f(glGetUniformLocation(this->ID, name), value.x, value.y, value.z);
}
void Shader::SetVec4f(c_string name, float x, float y, float z, float w, bool useShader)
{
	if (useShader)
		this->Use();
	glUniform4f(glGetUniformLocation(this->ID, name), x, y, z, w);
}
void Shader::SetVec4f(c_string name, const glm::vec4& value, bool useShader)
{
	if (useShader)
		this->Use();
	glUniform4f(glGetUniformLocation(this->ID, name), value.x, value.y, value.z, value.w);
}
void Shader::SetMat4(c_string name, const glm::mat4& matrix, bool useShader)
{
	if (useShader)
		this->Use();
	glUniformMatrix4fv(glGetUniformLocation(this->ID, name), 1, false, glm::value_ptr(matrix));
}
void Shader::SetMat3(c_string name, const glm::mat3& matrix, bool useShader)
{
	if (useShader)
		this->Use();
	glUniformMatrix3fv(glGetUniformLocation(this->ID, name), 1, false, glm::value_ptr(matrix));
}

void Shader::checkCompileErrors(uint object, std::string type)
{
	int success = 0;
	char infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(object, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(object, 1024, NULL, infoLog);
			throw std::exception(("| ERROR::SHADER: Compile-time error: Type: " + type + "\n" + 
								  infoLog + "\n -- --------------------------------------- --\n").c_str());
		}
	}
	else
	{
		glGetProgramiv(object, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(object, 1024, NULL, infoLog);
			throw std::exception(("| ERROR::SHADER: Link-time error: Type: " + type + "\n" +
								  infoLog + "\n -- --------------------------------------- --\n").c_str());
		}
	}
}