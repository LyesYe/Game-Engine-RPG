#ifndef MONSTER_H
#define MONSTER_H

#include "Character.h"

class Monster : public Character {
public:
    Monster(int gridX, int gridZ, glm::vec3 color, Map* map, const std::string& modelPath);
    virtual ~Monster() {}

    virtual std::string getClassName() const override { return "Monster"; }
    virtual void takeDamage(int damage, Character* attacker) override;

    bool shouldRetaliate() const;
    void retaliate();


    Character* lastAttacker = nullptr; 

};

#endif // MONSTER_H