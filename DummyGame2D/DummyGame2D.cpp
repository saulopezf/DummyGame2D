#include <iostream>
#include <string>
#include <iostream>
#include <sstream>
#include "Windows.h"
using namespace std;

#include "olcConsoleGameEngine.h"

const float gAccel = 12.0f;
const float gMaxVel = 12.0f;

const float playerAccel = 12.0f;
const float playerDecel = 6.0f;
const float playerMaxVel = 6.0f;

const float jumpForce = 8.5f;

struct velVector {
    float x, y;
};

struct point {
    float x, y;
};

struct hitbox {
    point topLeft;
    point topRight;
    point botLeft;
    point botRight;
};

class Player {
public:
    point position;
    bool onGround;
    bool jumping;

    Player(float x, float y) {
        position = { x, y };
        onGround = false;
        jumping = false;
    }

    void setPosition(float x, float y) {
        position = { x, y };
    }

    point nextMove(float velX, float velY, float elapsedTime) {
        return { position.x + velX * elapsedTime, position.y + velY * elapsedTime };
    }

    hitbox getHibox(point pos) {
        return {
            {pos.x,pos.y},
            {pos.x + 1.0f,pos.y},
            {pos.x,pos.y + 0.9f},
            {pos.x + 1.0f,pos.y + 0.9f},
        };
    }

};

Player player(0,0);

class OneLoneCoder_Plataformer : public olcConsoleGameEngine {
public: 
    OneLoneCoder_Plataformer() {
        m_sAppName = L"Juego de plataformas";
    }

private: 
    // Level properties
    wstring level;
    int levelWidth;
    int levelHeight;

    // Camera properties
    float cameraPosX = 0.0f;
    float cameraPosY = 0.0f;

    float velX = 0.0f;
    float velY = 0.0f;

protected:
    virtual bool OnUserCreate() {
        levelWidth = 64;
        levelHeight = 16;
        level += L"................................................................";
        level += L"................................................................";
        level += L"................................................................";
        level += L"................................................................";
        level += L"......#................########.................................";
        level += L"......................###..............#.#......................";
        level += L"....................###................#.#......................";
        level += L"...................####.........................................";
        level += L"####################################.##############.....########";
        level += L"...................................#.#...............###........";
        level += L"........................############.#............###...........";
        level += L"........................#............#.........###..............";
        level += L"........................#.############......###.................";
        level += L"........................#................###....................";
        level += L"........................#################.......................";
        level += L"................................................................";


        return true;
    }

    virtual bool OnUserUpdate(float elapsedTime) {
        // Lambdas
        auto tileInsideBoundries = [&](int x, int y) {
            return(x >= 0 && x < levelWidth && y >= 0 && y < levelHeight);
        };

        auto getTile = [&](int x, int y) {
            if (tileInsideBoundries(x, y))
                return level[y * levelWidth + x];
            else
                return L' ';
        };

        auto setTile = [&](int x, int y, wchar_t c) {
            if (tileInsideBoundries(x, y))
                level[y * levelWidth + x] = c;
        };

        auto checkCollision = [&](hitbox hitbox) {
            return getTile(hitbox.topLeft.x, hitbox.topLeft.y) != L'.'
                or getTile(hitbox.topRight.x, hitbox.topRight.y) != L'.'
                or getTile(hitbox.botLeft.x, hitbox.botLeft.y) != L'.'
                or getTile(hitbox.botRight.x, hitbox.botRight.y) != L'.';
            };

        stringstream a;
        a << "\nVelocity: " << velX << ", " << velY << "\n";
        OutputDebugStringA(a.str().c_str());
        OutputDebugString(player.onGround ? L"true" : L"false");

        // Input
        if (IsFocused()) {
            // JUMP
            if (GetKey(0x57).bPressed && player.onGround) {
                velY = -jumpForce;
                player.jumping = true;
                player.onGround = false;
                
            }

            // LEFT
            if (GetKey(0x41).bHeld) {
                velX += -playerAccel * elapsedTime;
            }

            // RIGHT
            if (GetKey(0x44).bHeld) {
                velX += playerAccel * elapsedTime;
            }
        }

        
        // Gravity
        velY += gAccel * elapsedTime;

        // Max gravity velocity
        if (velY > gMaxVel)
            velY = gMaxVel;
        // Max X velocity
        if (abs(velX) > playerMaxVel) {
            velX = (velX < 0) ? playerMaxVel * -1 : playerMaxVel;
        }

        if (player.onGround && velX != 0.0f) {
            float oldVelX = velX;
            velX += velX < 0
                ? (playerDecel * elapsedTime)
                : (playerDecel * elapsedTime) * -1;
            if ((oldVelX < 0 && velX > 0) or (oldVelX > 0 && velX < 0)) velX = 0;
        }


        point playerNextMove = player.nextMove(velX, velY, elapsedTime);

        // Horizontal collisions
        if (velX <= 0) {
            if (
                getTile(playerNextMove.x + 0.0f, player.position.y + 0.0f) != L'.' 
                or getTile(playerNextMove.x + 0.0f, player.position.y + 0.9f) != L'.'
            ) {
                playerNextMove.x = (int)playerNextMove.x + 1;
                velX = 0;
            }
        }
        else {
            if (getTile(playerNextMove.x + 1.0f, player.position.y + 0.0f) != L'.'
                or getTile(playerNextMove.x + 1.0f, player.position.y + 0.9f) != L'.'
            ) {
                playerNextMove.x = (int)playerNextMove.x;
                velX = 0;
            }
        }

        // Vertical collisions
        if (velY <= 0) {
            if (
                getTile(playerNextMove.x + 0.0f, playerNextMove.y) != L'.' 
                or getTile(playerNextMove.x + 0.9f, playerNextMove.y) != L'.'
            ) {
                playerNextMove.y = (int)playerNextMove.y + 1;
                velY = 0;
            }
        }
        else {
            if (
                getTile(playerNextMove.x + 0.0f, playerNextMove.y + 1.0f) != L'.' 
                or getTile(playerNextMove.x + 0.9f, playerNextMove.y + 1.0f) != L'.'
            ) {
                playerNextMove.y = (int)playerNextMove.y;
                velY = 0;
                player.onGround = true;
            }
        }

        player.setPosition(playerNextMove.x, playerNextMove.y);

        // Mover la camara con el jugador
        cameraPosX = player.position.x;
        cameraPosY = player.position.y;

        // Dibujar nivel
        int tileWidth = 8;
        int tileHeight = 8;
        int visibleTilesX = ScreenWidth() / tileWidth;
        int visibleTilesY = ScreenHeight() / tileHeight;

        // Calcular la ultima tile visible arriba izq
        float offsetX = cameraPosX - (float)visibleTilesX / 2.0f;
        float offsetY = cameraPosY - (float)visibleTilesY / 2.0f;

        // Parar la camara cuando el juego supera los bordes
        if (offsetX < 0) offsetX = 0;
        if (offsetY < 0) offsetY = 0;
        if (offsetX > levelWidth - visibleTilesX) offsetX = levelWidth - visibleTilesX;
        if (offsetY > levelHeight - visibleTilesY) offsetY = levelHeight - visibleTilesY;

        // Obtener los offsets de cada tile para un movimiento mas realista
        float tileOffsetX = (offsetX - (int)offsetX) * tileWidth;
        float tileOffsetY = (offsetY - (int)offsetY) * tileWidth;

        // Dibujar el mapa
        for (int x = -1; x < visibleTilesX + 1; x++) {
            for (int y = -1; y < visibleTilesY + 1; y++) {
                wchar_t tileId = getTile(x + offsetX, y + offsetY);

                switch (tileId) {
                    case L'.':
                        Fill(
                            x * tileWidth - tileOffsetX,
                            y * tileHeight - tileOffsetY,
                            (x + 1) * tileWidth - tileOffsetX,
                            (y + 1) * tileHeight - tileOffsetY,
                            PIXEL_SOLID,
                            FG_CYAN
                        );
                        break;
                    case L'#':
                        Fill(
                            x * tileWidth - tileOffsetX,
                            y * tileHeight - tileOffsetY,
                            (x + 1) * tileWidth - tileOffsetX,
                            (y + 1) * tileHeight - tileOffsetY,
                            PIXEL_SOLID,
                            FG_RED
                        );
                        break;
                    default:
                        Fill(
                            x * tileWidth - tileOffsetX,
                            y * tileHeight - tileOffsetY,
                            (x + 1) * tileWidth - tileOffsetX,
                            (y + 1) * tileHeight - tileOffsetY,
                            PIXEL_SOLID,
                            FG_BLACK
                        );
                        break;
                }
            }
        }
        //Fill(playerPosX * tileWidth, playerPosY * tileHeight, (playerPosX + 1) * tileWidth, (playerPosY + 1) * tileHeight, PIXEL_SOLID, FG_GREEN);
        Fill(
            (player.position.x - offsetX) * tileWidth,
            (player.position.y - offsetY) * tileWidth,
            (player.position.x - offsetX + 1.0f) * tileHeight,
            (player.position.y - offsetY + 1.0f) * tileHeight,
            PIXEL_SOLID,
            FG_GREEN
        );

        return true;
    }
};

int main()
{
    OneLoneCoder_Plataformer game;
    if (game.ConstructConsole(160, 120, 8, 8))
        game.Start();

    return 0;
}
