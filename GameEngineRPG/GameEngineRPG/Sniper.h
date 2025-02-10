#ifndef SNIPER_H
#define SNIPER_H

#include "Adventurer.h"

class Sniper : public Adventurer {
public:
    Sniper(int gridX, int gridZ, Map* map);
    ~Sniper() {}

    void updateTurn(); 
    virtual std::string getClassName() const override { return "Sniper"; }

private:
    int turnsSinceLastAttack = 0;
    int attackCooldownTurns = 2; 
};

#endif // SNIPER_H