#ifndef RENDERER_H
#define RENDERER_H

#include <glad/glad.h>
#include "Camera.h"
#include "Map.h"

class Renderer {
public:
    unsigned int shaderProgram;

    Renderer();
    bool InitShaders(const char* vertexShaderSource, const char* fragmentShaderSource);
    void Clear();
    void RenderMap(Map& map, const Camera& camera);
    void UseShader(); // Helper to use the shader program

private:
    unsigned int vertexShader;
    unsigned int fragmentShader;
};

#endif // RENDERER_H