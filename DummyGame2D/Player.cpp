#include "Utils.h"
#include "Player.h"

Player::Player(float x, float y) {
    position = { x, y };
    velocity = { 0, 0 };
    onGround = false;
    jumping = false;
}

void Player::setPosition(float x, float y) {
    position = { x, y };
}

point Player::nextMove(float elapsedTime) {
    return { position.x + velocity.x * elapsedTime, position.y + velocity.y * elapsedTime };
}

hitbox Player::getHibox(point pos) {
    return {
        {pos.x,pos.y},
        {pos.x + 1.0f,pos.y},
        {pos.x,pos.y + 0.9f},
        {pos.x + 1.0f,pos.y + 0.9f},
    };
}