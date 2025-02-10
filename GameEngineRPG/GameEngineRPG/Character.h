#ifndef CHARACTER_H
#define CHARACTER_H

#include <glm/glm.hpp>
#include "Camera.h"
#include <string>
#include <vector>
#include <chrono>

class Map; 
class Character; 

class Character {
public:
    Character(int gridX, int gridZ, glm::vec3 color, Map* map, const std::string& modelPath);
    virtual ~Character() {}
    virtual void render(unsigned int shaderProgram, const Camera& camera);
    virtual void takeDamage(int damage, Character* attacker = nullptr); 
    bool isAlive() const { return health > 0; }
    void moveTowards(int targetGridX, int targetGridZ, int minDistanceToTarget);
    virtual void attack() {} 

    int getGridX() const { return gridX; }
    int getGridZ() const { return gridZ; }
    glm::vec3 getPosition() const { return position; }
    int getHealth() const { return health; }
    int getBaseDamage() const { return baseDamage; }
    int getAttackRange() const { return attackRange; }
    virtual std::string getClassName() const { return "Character"; }
    std::string getModelPath() const { return modelPath; } 


    glm::vec3 position;
    glm::vec3 color;
    int gridX, gridZ;
    Map* map;
    int health;
    int baseDamage;
    int attackRange;
    float moveSpeed;
    glm::vec3 targetPosition;
    unsigned int characterTextureID;


protected:
    unsigned int VAO, VBO, EBO;
    virtual void setupMesh();
    std::string modelPath;
    std::vector<unsigned int> indices;

private:
    glm::vec3 lastMoveDirection = glm::vec3(0.0f, 0.0f, 1.0f);
    float currentRotationAngleY = 0.0f;
    glm::vec3 originalRenderPosition;
};

#endif // CHARACTER_H