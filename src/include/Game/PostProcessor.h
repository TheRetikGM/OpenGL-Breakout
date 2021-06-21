#ifndef __POSTPROCESSOR_H__
#define __POSTPROCCESOR_H__

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "../Texture.h"
#include "../SpriteRenderer.h"
#include "../Shader.h"

class PostProcessor
{
public:
    Shader PostProcessingShader;
    Texture2D Texture;
    unsigned int Width, Height;

    bool shake, confuse, chaos;

    PostProcessor(Shader shader, unsigned int width, unsigned int height);

    void BeginRender();
    void EndRender();
    void Render(float time);
private:
    unsigned int MSFBO, FBO;
    unsigned int RBO;
    unsigned int VAO;

    void initRenderData();
};

#endif