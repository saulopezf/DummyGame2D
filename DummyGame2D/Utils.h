#pragma once

const float gAccel = 12.0f;
const float gMaxVel = 12.0f;

const float playerAccel = 12.0f;
const float playerDecel = 6.0f;
const float playerMaxVel = 6.0f;

const float jumpForce = 8.5f;

struct point {
    float x, y;
};

struct hitbox {
    point topLeft;
    point topRight;
    point botLeft;
    point botRight;
};