#include "Renderer.h"
#include <iostream>

Renderer::Renderer() : shaderProgram(0), vertexShader(0), fragmentShader(0) {}

// ---------- Initialisation des Shaders ----------
bool Renderer::InitShaders(const char* vertexShaderSource, const char* fragmentShaderSource) {
    // ---------- Compilation du Vertex Shader ----------
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        return false;
    }

    // ---------- Compilation du Fragment Shader ----------
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        return false;
    }

    // ---------- Lien du Programme Shader ----------
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        return false;
    }

    // ---------- Suppression des objets shaders après linkage ----------
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return true;
}

// ---------- Nettoyage de l'écran ----------
void Renderer::Clear() {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

// ---------- Rendu de la carte ----------
void Renderer::RenderMap(Map& map, const Camera& camera) {
    map.render(shaderProgram, camera);
}

// ---------- Utilisation du Shader ----------
void Renderer::UseShader() {
    glUseProgram(shaderProgram);
}