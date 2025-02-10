#include "Mage.h"
#include "Map.h"
#include <glm/glm.hpp>
#include <iostream> 

Mage::Mage(int gridX, int gridZ, Map* map)
    : Adventurer(gridX, gridZ, glm::vec3(0.0f, 0.0f, 1.0f), map, "Assets/Characters/gltf/Demon.gltf")
{
    attackRange = 2; 
    baseDamage = 8;             
    health = 80;                
}

