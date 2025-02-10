#include "Adventurer.h"
#include "Map.h"
#include <glm/glm.hpp>

Adventurer::Adventurer(int gridX, int gridZ, glm::vec3 color, Map* map, const std::string& modelPath) 
    : Character(gridX, gridZ, color, map, modelPath) 
{
}