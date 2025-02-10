#ifndef MAP_H
#define MAP_H

#include <vector>
#include <glm/glm.hpp>
#include "Camera.h" // Forward declare or include if needed
#include "Character.h" // Include Character header - needed for cell occupancy

class Map {
public:
    Map(int gridSize = 8, float cellSize = 1.0f);
    ~Map();

    void generateChessboard();
    void render(unsigned int shaderProgram, const Camera& camera);

    // **Public Getter for cellSize**
    float getCellSize() const { return cellSize; }
    int getGridSize() const { return gridSize; }   // **New: Public Getter for gridSize**
    unsigned int grassTexture; // **New: Texture ID for grass**
    unsigned int dirtTexture;  // **New: Texture ID for dirt**

    // **Cell Occupancy System - Public Methods**
    bool isCellOccupied(int gridX, int gridZ) const;
    bool setCellOccupied(int gridX, int gridZ, Character* character);
    void setCellFree(int gridX, int gridZ);
    void addCharacterToMap(Character* character, int gridX, int gridZ);
    void removeCharacterFromMap(Character* character, int gridX, int gridZ);


private:
    int gridSize;
    float cellSize;
    std::vector<std::vector<glm::vec3>> cellColors;
    unsigned int loadTexture(const char* path); // **New: Helper function to load texture**
    std::vector<std::vector<int>> cellTextureTypes;
    std::vector<std::vector<Character*>> occupiedCells; // **<-- ADD THIS LINE: Cell occupancy grid**


    unsigned int VAO, VBO, EBO;
    void setupMesh();
};

#endif