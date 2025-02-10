#ifndef WARRIOR_H
#define WARRIOR_H

#include "Adventurer.h"

class Warrior : public Adventurer {
public:
    Warrior(int gridX, int gridZ, Map* map);
    ~Warrior() {}

    virtual std::string getClassName() const override { return "Warrior"; }
};

#endif // WARRIOR_H