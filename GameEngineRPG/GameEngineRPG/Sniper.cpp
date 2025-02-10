
#include "Sniper.h"
#include "Map.h"
#include <glm/glm.hpp>
#include <iostream> // For debug output

Sniper::Sniper(int gridX, int gridZ, Map* map)
    : Adventurer(gridX, gridZ, glm::vec3(0.0f, 1.0f, 0.0f), map, "Assets/Characters/gltf/Character_Female_2.gltf")
{
    attackRange = 3; 
    baseDamage = 15;            
    health = 70;               
    turnsSinceLastAttack = 0;  
}


void Sniper::updateTurn() {
    turnsSinceLastAttack++; 
    if (turnsSinceLastAttack > attackCooldownTurns) {
        turnsSinceLastAttack = attackCooldownTurns; 
    }
}