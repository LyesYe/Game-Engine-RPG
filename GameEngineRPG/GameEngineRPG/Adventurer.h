#ifndef ADVENTURER_H
#define ADVENTURER_H

#include "Character.h"

class Adventurer : public Character {
public:
    Adventurer(int gridX, int gridZ, glm::vec3 color, Map* map, const std::string& modelPath);
    virtual ~Adventurer() {}

    virtual std::string getClassName() const override { return "Adventurer"; }
};

#endif // ADVENTURER_H