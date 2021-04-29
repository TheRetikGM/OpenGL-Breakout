#pragma once
#include "CustomTypes.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <exception>

class Shader
{
public:
    uint ID = 0;

    Shader() { }    

    Shader &Use();

    void Compile(c_string vertexSource, c_string fragmentSource, c_string geometrySource = nullptr);

    void SetFloat   (c_string name, float value, bool useShader = false);
    void SetInt     (c_string name, int value, bool useShader = false);
    void SetVec2f   (c_string name, float x, float y, bool useShader = false);
    void SetVec2f   (c_string name, const glm::vec2& value, bool useShader = false);
    void SetVec3f   (c_string name, float x, float y, float z, bool useShader = false);
    void SetVec3f   (c_string name, const glm::vec3& value, bool useShader = false);
    void SetVec4f   (c_string name, float x, float y, float z, float w, bool useShader = false);
    void SetVec4f   (c_string name, const glm::vec4& value, bool useShader = false);
    void SetMat4    (c_string name, const glm::mat4& matrix, bool useShader = false);
    void SetMat3    (c_string name, const glm::mat3& matrix, bool useShader = false);
private:
    void checkCompileErrors(uint object, std::string type);
};