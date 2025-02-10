#include "Monster.h"
#include "Map.h"
#include <glm/glm.hpp>
#include <iostream> 

Monster::Monster(int gridX, int gridZ, glm::vec3 color, Map* map, const std::string& modelPath) 
    : Character(gridX, gridZ, color, map, modelPath) 
{
}

void Monster::takeDamage(int damage, Character* attacker) {
    Character::takeDamage(damage, attacker); 
    if (health > 0 && attacker != nullptr && attacker->isAlive()) { 
        lastAttacker = attacker; 
        std::cout << getClassName() << " at (" << gridX << ", " << gridZ << ") remembers attacker: " << lastAttacker->getClassName() << " at (" << lastAttacker->getGridX() << ", " << lastAttacker->getGridZ() << ")" << std::endl;
    }
    else {
        lastAttacker = nullptr; 
    }
}


bool Monster::shouldRetaliate() const {
    return lastAttacker != nullptr && lastAttacker->isAlive();
}

void Monster::retaliate() {
    if (lastAttacker != nullptr && lastAttacker->isAlive() && map != nullptr) {
        moveTowards(lastAttacker->getGridX(), lastAttacker->getGridZ(), 0);
    }
}