#define GLM_ENABLE_EXPERIMENTAL

#include "Character.h"
#include <glad/glad.h>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include "Map.h"
#include <cmath>
#include <algorithm>
#include <chrono>
#include <thread>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include <stb_image.h>
#include <unordered_map>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <cmath>

std::unordered_map<std::string, unsigned int> textureCache;

unsigned int TextureFromAssimpData(aiTexture* texture, const std::string& modelPath);

Character::Character(int gridX, int gridZ, glm::vec3 color, Map* map, const std::string& modelPath)
    : gridX(gridX), gridZ(gridZ), color(color), map(map), modelPath(modelPath),
    health(100),
    baseDamage(5),
    attackRange(1),
    moveSpeed(2.0f),
    characterTextureID(0)
{
    if (map) {
        position = glm::vec3(gridX * map->getCellSize(), 0.5f, gridZ * map->getCellSize());
    }
    else {
        position = glm::vec3(0.0f);
    }
    targetPosition = position;
    setupMesh();
}

glm::vec3 lerp(const glm::vec3& start, const glm::vec3& end, float t) {
    return start + (end - start) * t;
}

void Character::takeDamage(int damage, Character* attacker) {
    health -= damage;
    if (health <= 0) {
        health = 0;
        std::cout << getClassName() << " à (" << gridX << ", " << gridZ << ") est mort!" << std::endl;
    }
    else {
        std::cout << getClassName() << " à (" << gridX << ", " << gridZ << ") a pris " << damage << " dégâts. Santé: " << health << std::endl;
        glm::vec3 originalColor = color;
        color = glm::vec3(1.0f, 0.0f, 0.0f);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        color = originalColor;
    }
}

void Character::render(unsigned int shaderProgram, const Camera& camera) {
    glUseProgram(shaderProgram);

    glm::mat4 projection = camera.GetProjectionMatrix();
    glm::mat4 view = camera.GetViewMatrix();
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &projection[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, &view[0][0]);

    float interpolationFactor = 0.005f;
    position = lerp(position, targetPosition, interpolationFactor);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);

    glm::vec3 currentMoveDirection = targetPosition - position;
    if (glm::length(currentMoveDirection) > 0.01f) {

        glm::vec3 normalizedMoveDir = glm::normalize(currentMoveDirection);

        if (glm::dot(normalizedMoveDir, lastMoveDirection) < 0.999f) {
            currentRotationAngleY = std::atan2(normalizedMoveDir.x, normalizedMoveDir.z);
            lastMoveDirection = normalizedMoveDir;
            std::cout << "**DEBUG - Direction de Rotation Changée, Nouvel Angle (radians):** " << currentRotationAngleY << std::endl;
        }
    }

    glm::quat rotationQuatY = glm::angleAxis(currentRotationAngleY, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 rotationMatrixY = glm::toMat4(rotationQuatY);
    model = model * rotationMatrixY;

    model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &model[0][0]);

    unsigned int objectColorLoc = glGetUniformLocation(shaderProgram, "objectColor");
    glUniform3fv(objectColorLoc, 1, &color[0]);

    unsigned int cameraPosLoc = glGetUniformLocation(shaderProgram, "cameraPos");
    glUniform3fv(cameraPosLoc, 1, &camera.Position[0]);

    if (characterTextureID != 0) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, characterTextureID);
        unsigned int textureLoc = glGetUniformLocation(shaderProgram, "characterTexture");
        glUniform1i(textureLoc, 0);
    }

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}


void Character::setupMesh() {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(modelPath,
        aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return;
    }
    std::cout << "Scène Assimp chargée avec succès depuis: " << modelPath << std::endl;

    aiMesh* mesh = scene->mMeshes[0];
    std::cout << "Traitement du mesh: " << mesh->mName.C_Str() << std::endl;
    std::cout << "Nombre de vertex: " << mesh->mNumVertices << std::endl;
    std::cout << "Nombre de faces: " << mesh->mNumFaces << std::endl;
    std::cout << "A des normales: " << (mesh->HasNormals() ? "Oui" : "Non") << std::endl;
    std::cout << "A des coordonnées de texture: " << (mesh->HasTextureCoords(0) ? "Oui" : "Non") << std::endl;


    std::vector<float> vertices;
    indices.clear();
    std::vector<float> normals;
    std::vector<float> texCoords;

    // ---------- Chargement des données de vertex, normales et coordonnées de texture ----------
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        vertices.push_back(mesh->mVertices[i].x);
        vertices.push_back(mesh->mVertices[i].y);
        vertices.push_back(mesh->mVertices[i].z);

        if (mesh->HasNormals()) {
            normals.push_back(mesh->mNormals[i].x);
            normals.push_back(mesh->mNormals[i].y);
            normals.push_back(mesh->mNormals[i].z);
        }

        if (mesh->mTextureCoords[0]) {
            texCoords.push_back(mesh->mTextureCoords[0][i].x);
            texCoords.push_back(mesh->mTextureCoords[0][i].y);
        }
        else {
            texCoords.push_back(0.0f);
            texCoords.push_back(0.0f);
        }
    }

    // ---------- Chargement des indices ----------
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }
    std::cout << "Nombre d'indices: " << indices.size() << std::endl;


    characterTextureID = 0;
    if (mesh->mMaterialIndex >= 0) {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        aiString texturePath;
        if (AI_SUCCESS == material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath)) {
            std::string fullTexturePath = texturePath.C_Str();

            std::cout << "**DEBUG - setupMesh() - Chemin de texture Assimp brut:** " << texturePath.C_Str() << std::endl;
            std::cout << "**DEBUG - setupMesh() - Chemin de texture traité (avant chargement de texture):** " << fullTexturePath << std::endl;


            if (fullTexturePath.length() > 0 && fullTexturePath[0] == '*') {
                int textureIndex = std::atoi(fullTexturePath.c_str() + 1);
                if (textureIndex >= 0 && textureIndex < scene->mNumTextures) {
                    std::cout << "**DEBUG - setupMesh() - Texture intégrée détectée, Index: " << textureIndex << std::endl;
                    characterTextureID = TextureFromAssimpData(scene->mTextures[textureIndex], modelPath);
                }
                else {
                    std::cerr << "ERROR - setupMesh() - Index de texture intégrée invalide: " << textureIndex << std::endl;
                    characterTextureID = 0;
                }
            }
        }
        else {
            std::cout << "Aucune texture diffuse trouvée dans le matériau, utilisation d'une couleur unie." << std::endl;
        }
    }
    else {
        std::cout << "Aucun matériau assigné au mesh, utilisation d'une couleur unie." << std::endl;
    }


    // ---------- Configuration des VBOs, VAO et EBO ----------
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    if (!normals.empty()) {
        unsigned int normalVBO;
        glGenBuffers(1, &normalVBO);
        glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
        glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), &normals[0], GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
    }

    unsigned int texCoordVBO;
    glGenBuffers(1, &texCoordVBO);
    glBindBuffer(GL_ARRAY_BUFFER, texCoordVBO);
    glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(float), &texCoords[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(2);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    glBindVertexArray(0);
}


unsigned int TextureFromAssimpData(aiTexture* texture, const std::string& modelPath) {
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = nullptr;

    if (texture->mHeight == 0) {
        std::cout << "**DEBUG - TextureFromAssimpData - Chargement de texture compressée." << std::endl;
        data = stbi_load_from_memory(reinterpret_cast<unsigned char*>(texture->pcData), texture->mWidth, &width, &height, &nrComponents, 0);
    }
    else
    {
        std::cout << "**DEBUG - TextureFromAssimpData - Chargement de texture non compressée." << std::endl;
        data = stbi_load_from_memory(reinterpret_cast<unsigned char*>(texture->pcData), texture->mWidth * texture->mHeight, &width, &height, &nrComponents, 0);
    }


    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;
        else
            format = GL_RGB;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
        std::cout << "Texture chargée avec succès depuis les données Assimp, ID de texture: " << textureID << std::endl;
    }
    else
    {
        std::cerr << "Texture non chargée depuis les données Assimp." << std::endl;
        stbi_image_free(data);
        return 0;
    }

    return textureID;
}


void Character::moveTowards(int targetGridX, int targetGridZ, int minDistanceToTarget) {
    if (map == nullptr) return;

    int deltaX = targetGridX - gridX;
    int deltaZ = targetGridZ - gridZ;

    float currentDistance = std::abs(deltaX) + std::abs(deltaZ);
    if (currentDistance <= minDistanceToTarget) {
        return;
    }

    int moveX = 0;
    int moveZ = 0;

    if (std::abs(deltaX) > std::abs(deltaZ)) {
        moveX = (deltaX > 0) ? 1 : -1;
    }
    else if (std::abs(deltaZ) > std::abs(deltaX)) {
        moveZ = (deltaZ > 0) ? 1 : -1;
    }
    else {
        moveX = (deltaX > 0) ? 1 : (deltaX < 0 ? -1 : 0);
        moveZ = (deltaZ > 0) ? 1 : (deltaZ < 0 ? -1 : 0);
    }

    int newGridX = gridX + moveX;
    int newGridZ = gridZ + moveZ;

    if (newGridX >= 0 && newGridX < map->getGridSize() &&
        newGridZ >= 0 && newGridZ < map->getGridSize()) {

        float nextDistance = std::abs(targetGridX - newGridX) + std::abs(targetGridZ - newGridZ);
        if (nextDistance <= minDistanceToTarget) {
            return;
        }

        // ---------- Vérification de l'occupation de la cellule - Avant le mouvement ----------
        if (!map->isCellOccupied(newGridX, newGridZ)) {

            map->setCellFree(gridX, gridZ);

            gridX = newGridX;
            gridZ = newGridZ;
            targetPosition = glm::vec3(gridX * map->getCellSize(), 0.5f, gridZ * map->getCellSize());

            map->setCellOccupied(gridX, gridZ, this);
        }
        else {
            // ---------- La cellule est occupée, ne pas se déplacer ----------
            std::cout << getClassName() << " à (" << gridX << ", " << gridZ << ") a essayé de se déplacer vers la cellule occupée ("
                << newGridX << ", " << newGridZ << "), mouvement bloqué." << std::endl;
            return;
        }
    }
    else {
        return;
    }
}