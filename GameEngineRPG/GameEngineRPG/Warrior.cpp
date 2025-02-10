#include "Warrior.h"
#include "Map.h"
#include <glm/glm.hpp>

Warrior::Warrior(int gridX, int gridZ, Map* map)
    : Adventurer(gridX, gridZ, glm::vec3(0.0f, 0.5f, 0.0f), map, "Assets/Characters/gltf/Character_Male_1.gltf") // Pass Warrior model path here
{
    baseDamage = 10;  
    attackRange = 1;  
    health = 120;     
}