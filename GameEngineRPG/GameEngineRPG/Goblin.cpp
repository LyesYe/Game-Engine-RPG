#include "Goblin.h"
#include "Map.h"
#include <glm/glm.hpp>

Goblin::Goblin(int gridX, int gridZ, Map* map)
    : Monster(gridX, gridZ, glm::vec3(0.5f, 0.0f, 0.0f), map, "Assets/Characters/gltf/Goblin.gltf") 
{
    baseDamage = 6;  
    attackRange = 1; 
    health = 80;      
}