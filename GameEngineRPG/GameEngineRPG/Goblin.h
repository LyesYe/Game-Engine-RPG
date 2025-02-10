#ifndef GOBLIN_H
#define GOBLIN_H

#include "Monster.h"

class Goblin : public Monster {
public:
    Goblin(int gridX, int gridZ, Map* map);
    ~Goblin() {}

    virtual std::string getClassName() const override { return "Monster"; }


};

#endif // GOBLIN_H