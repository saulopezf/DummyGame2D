#pragma once
#include "Utils.h"

class Player {
public:
    point position;
    point velocity;
    bool onGround;
    bool jumping;

    Player(float x, float y);

    void setPosition(float x, float y);
    point nextMove(float elapsedTime);
    hitbox getHibox(point pos);
};