#ifndef MAGE_H
#define MAGE_H

#include "Adventurer.h"

class Mage : public Adventurer {
public:
    Mage(int gridX, int gridZ, Map* map);
    ~Mage() {}

    virtual std::string getClassName() const override { return "Warrior"; }


private:
    int mageAttackRange = 2; 
};

#endif // MAGE_H