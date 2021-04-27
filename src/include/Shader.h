#pragma once
typedef const char* c_string;
typedef unsigned int uint;

class Shader
{
    uint ID;

    Shader() { }

    Shader &Use();

    void Compile(c_string vertexSource, c_string fragmentSource, c_string geometrySource = nullptr);

    void SetFloat();
    void SetInt();
    void SetVec2f();
    void SetVec2f();
    void SetVec3f();
    void SetVec3f();
    void SetVec4f();
    void SetVec4f();
    void SetMat4();
    void SetMat3();
};