#include "Map.h"
#include <glad/glad.h>
#include <vector>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Map::Map(int gridSize, float cellSize)
    : gridSize(gridSize), cellSize(cellSize)
{
    cellColors.resize(gridSize, std::vector<glm::vec3>(gridSize));
    cellTextureTypes.resize(gridSize, std::vector<int>(gridSize));
    occupiedCells.resize(gridSize, std::vector<Character*>(gridSize, nullptr));
    generateChessboard();
    setupMesh();

    grassTexture = loadTexture("grass_texture.png");
    dirtTexture = loadTexture("dirt_texture.png");

    std::cout << "Grass Texture ID: " << grassTexture << std::endl;
    std::cout << "Dirt Texture ID: " << dirtTexture << std::endl;
}

Map::~Map()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteTextures(1, &grassTexture);
    glDeleteTextures(1, &dirtTexture);

}

unsigned int Map::loadTexture(const char* path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    std::string texturePath = "textures/";
    texturePath += path;
    unsigned char* data = stbi_load(texturePath.c_str(), &width, &height, &nrComponents, 0);

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
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }
    return textureID;
}


void Map::generateChessboard() {
    cellColors.resize(gridSize, std::vector<glm::vec3>(gridSize));

    float grassProbability = 0.7f;

    for (int i = 0; i < gridSize; ++i) {
        for (int j = 0; j < gridSize; ++j) {
            float randomValue = static_cast<float>(rand()) / RAND_MAX;

            if (randomValue < grassProbability) {
                cellTextureTypes[i][j] = 0;
            }
            else {
                cellTextureTypes[i][j] = 1;
            }
        }
    }

    for (int i = 1; i < gridSize - 1; ++i) {
        for (int j = 1; j < gridSize - 1; ++j) {
            int grassNeighbors = 0;
            for (int xOffset = -1; xOffset <= 1; ++xOffset) {
                for (int yOffset = -1; yOffset <= 1; ++yOffset) {
                    if (xOffset == 0 && yOffset == 0) continue;
                    if (cellTextureTypes[i + xOffset][j + yOffset] == 0) {
                        grassNeighbors++;
                    }
                }
            }
            if (cellTextureTypes[i][j] == 1 && grassNeighbors >= 5) {
                cellTextureTypes[i][j] = 0;
            }
        }
    }
}



void Map::render(unsigned int shaderProgram, const Camera& camera) {
    glUseProgram(shaderProgram);

    glm::mat4 projection = camera.GetProjectionMatrix();
    glm::mat4 view = camera.GetViewMatrix();
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &projection[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, &view[0][0]);

    glBindVertexArray(VAO);
    for (int i = 0; i < gridSize; ++i) {
        for (int j = 0; j < gridSize; ++j) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(i * cellSize, -cellSize, j * cellSize));
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &model[0][0]);

            if (cellTextureTypes[i][j] == 0) {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, grassTexture);
                glUniform1i(glGetUniformLocation(shaderProgram, "textureType"), 0);
            }
            else if (cellTextureTypes[i][j] == 1) {
                glActiveTexture(GL_TEXTURE0 + 1);
                glBindTexture(GL_TEXTURE_2D, dirtTexture);
                glUniform1i(glGetUniformLocation(shaderProgram, "textureType"), 1);
            }
            glUniform3fv(glGetUniformLocation(shaderProgram, "lightColor"), 1, &glm::vec3(1.0f, 1.0f, 1.0f)[0]);

            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        }
    }
    glBindVertexArray(0);
}



void Map::setupMesh() {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    for (int i = 0; i < gridSize; ++i) {
        for (int j = 0; j < gridSize; ++j) {
            float x = i * cellSize;
            float z = j * cellSize;
            float halfCell = cellSize / 2.0f;

            float cellVertices[] = {
                x - halfCell, 0.0f, z - halfCell,   0.0f, 1.0f, 0.0f,    0.0f, 0.0f,
                x + halfCell, 0.0f, z - halfCell,   0.0f, 1.0f, 0.0f,    1.0f, 0.0f,
                x + halfCell, 0.0f, z + halfCell,   0.0f, 1.0f, 0.0f,    1.0f, 1.0f,
                x - halfCell, 0.0f, z + halfCell,   0.0f, 1.0f, 0.0f,    0.0f, 1.0f,
                x - halfCell, -cellSize, z - halfCell, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
                x + halfCell, -cellSize, z - halfCell, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
                x + halfCell, -cellSize, z + halfCell, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
                x - halfCell, -cellSize, z + halfCell, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f
            };
            for (float val : cellVertices) {
                vertices.push_back(val);
            }

            unsigned int cellIndices[] = {
                0, 1, 2,  2, 3, 0,
                1, 5, 6,  6, 2, 1,
                5, 4, 7,  7, 6, 5,
                4, 0, 3,  3, 7, 4,
                3, 2, 6,  6, 7, 3,
                4, 5, 1,  1, 0, 4
            };
            unsigned int indexOffset = 8 * (i * gridSize + j);
            for (unsigned int index : cellIndices) {
                indices.push_back(index + indexOffset);
            }
        }
    }


    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);


    glBindVertexArray(0);
}

// ---------- Système d'occupation des cellules ----------
bool Map::isCellOccupied(int gridX, int gridZ) const {
    if (gridX >= 0 && gridX < gridSize && gridZ >= 0 && gridZ < gridSize) {
        return occupiedCells[gridX][gridZ] != nullptr;
    }
    return true;
}

bool Map::setCellOccupied(int gridX, int gridZ, Character* character) {
    if (gridX >= 0 && gridX < gridSize && gridZ >= 0 && gridZ < gridSize) {
        if (!occupiedCells[gridX][gridZ]) {
            occupiedCells[gridX][gridZ] = character;
            return true;
        }
    }
    return false;
}

void Map::setCellFree(int gridX, int gridZ) {
    if (gridX >= 0 && gridX < gridSize && gridZ >= 0 && gridZ < gridSize) {
        occupiedCells[gridX][gridZ] = nullptr;
    }
}

void Map::addCharacterToMap(Character* character, int gridX, int gridZ) {
    setCellOccupied(gridX, gridZ, character);
}

void Map::removeCharacterFromMap(Character* character, int gridX, int gridZ) {
    setCellFree(gridX, gridZ);
}
