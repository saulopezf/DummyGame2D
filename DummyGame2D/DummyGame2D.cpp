#include <iostream>
#include <string>

#include "olcConsoleGameEngine.h"
#include "Utils.h"
#include "Player.h"

using namespace std;

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

    // Sprites
    olcSprite *spriteMan = nullptr;
    olcSprite* spriteTiles = nullptr;

    // Flags para las animaciones
    int sprX = 0;
    int sprY = 0;

    Player player = Player(0, 0);


protected:
    virtual bool OnUserCreate() {
        levelWidth = 64;
        levelHeight = 16;
        level += L"................................................................";
        level += L"................................................................";
        level += L".......oooo.....................................................";
        level += L"........oo......................................................";
        level += L".........................######.................................";
        level += L".....bb?bb?bb..........####............#.#......................";
        level += L".....................######............#.#......................";
        level += L"...................########.....................................";
        level += L"####################################.##############.....########";
        level += L"...................................#.#...............###........";
        level += L"........................############.#............###...........";
        level += L"........................#............#.........###..............";
        level += L"........................#.############......###.................";
        level += L"........................#................###....................";
        level += L"........................#################.......................";
        level += L"................................................................";

        spriteMan = new olcSprite(L"./minijario.spr");
        spriteTiles = new olcSprite(L"./leveljario.spr");

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

        auto checkCollision = [&](hitbox hitbox, wchar_t tile) {
            return getTile(hitbox.topLeft.x, hitbox.topLeft.y) == tile
                or getTile(hitbox.topRight.x, hitbox.topRight.y) == tile
                or getTile(hitbox.botLeft.x, hitbox.botLeft.y) == tile
                or getTile(hitbox.botRight.x, hitbox.botRight.y) == tile;
            };

        // Input
        if (IsFocused()) {
            // JUMP
            if (GetKey(0x57).bPressed && player.onGround) {
                player.velocity.y = -jumpForce;
                player.jumping = true;
                player.onGround = false;
                sprX = 1;
            }

            // LEFT
            if (GetKey(0x41).bHeld) {
                player.velocity.x += -playerAccel * elapsedTime;
                sprY = 1;
            }

            // RIGHT
            if (GetKey(0x44).bHeld) {
                player.velocity.x += playerAccel * elapsedTime;
                sprY = 0;
            }
        }
        
        // Gravity
        player.velocity.y += gAccel * elapsedTime;

        // Max gravity velocity
        if (player.velocity.y > gMaxVel)
            player.velocity.y = gMaxVel;

        // Max X velocity
        if (abs(player.velocity.x) > playerMaxVel) {
            player.velocity.x = (player.velocity.x < 0) ? playerMaxVel * -1 : playerMaxVel;
        }

        // Deceleracion cuando estas en el suelo
        if (player.onGround && player.velocity.x != 0.0f) {
            float oldVelX = player.velocity.x;
            player.velocity.x += player.velocity.x < 0
                ? (playerDecel * elapsedTime)
                : (playerDecel * elapsedTime) * -1;
            if ((oldVelX < 0 && player.velocity.x > 0) or (oldVelX > 0 && player.velocity.x < 0)) player.velocity.x = 0;
        }

        point playerNextMove = player.nextMove(elapsedTime);

        // Coger monedas
        if (getTile(playerNextMove.x + 0.0f, playerNextMove.y + 0.0f) == L'o')
            setTile(playerNextMove.x + 0.0f, playerNextMove.y + 0.0f, L'.');
        if (getTile(playerNextMove.x + 0.0f, playerNextMove.y + 1.0f) == L'o')
            setTile(playerNextMove.x + 0.0f, playerNextMove.y + 1.0f, L'.');
        if (getTile(playerNextMove.x + 1.0f, playerNextMove.y + 0.0f) == L'o')
            setTile(playerNextMove.x + 1.0f, playerNextMove.y + 0.0f, L'.');
        if (getTile(playerNextMove.x + 1.0f, playerNextMove.y + 1.0f) == L'o')
            setTile(playerNextMove.x + 1.0f, playerNextMove.y + 1.0f, L'.');

        // Horizontal collisions
        if (player.velocity.x <= 0) {
            if (
                getTile(playerNextMove.x + 0.0f, player.position.y + 0.0f) != L'.' 
                or getTile(playerNextMove.x + 0.0f, player.position.y + 0.9f) != L'.'
            ) {
                playerNextMove.x = (int)playerNextMove.x + 1;
                player.velocity.x = 0;
            }
        }
        else {
            if (getTile(playerNextMove.x + 1.0f, player.position.y + 0.0f) != L'.'
                or getTile(playerNextMove.x + 1.0f, player.position.y + 0.9f) != L'.'
            ) {
                playerNextMove.x = (int)playerNextMove.x;
                player.velocity.x = 0;
            }
        }

        // Vertical collisions
        if (player.velocity.y <= 0) {
            if (
                getTile(playerNextMove.x + 0.0f, playerNextMove.y) != L'.' 
                or getTile(playerNextMove.x + 0.9f, playerNextMove.y) != L'.'
            ) {
                playerNextMove.y = (int)playerNextMove.y + 1;
                player.velocity.y = 0;
            }
        }
        else {
            if (
                getTile(playerNextMove.x + 0.0f, playerNextMove.y + 1.0f) != L'.' 
                or getTile(playerNextMove.x + 0.9f, playerNextMove.y + 1.0f) != L'.'
            ) {
                playerNextMove.y = (int)playerNextMove.y;
                player.velocity.y = 0;
                player.onGround = true;
                sprX = 0;
            }
        }

        player.setPosition(playerNextMove.x, playerNextMove.y);

        // Mover la camara con el jugador
        cameraPosX = player.position.x;
        cameraPosY = player.position.y;

        // Dibujar nivel
        int tileWidth = 16;
        int tileHeight = 16;
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
                        DrawPartialSprite(
                            x * tileWidth - tileOffsetX,
                            y * tileHeight - tileOffsetY,
                            spriteTiles,
                            2 * tileWidth,
                            0 * tileHeight,
                            tileWidth, tileHeight);
                        break;
                    case L'b':
                        DrawPartialSprite(
                            x * tileWidth - tileOffsetX,
                            y * tileHeight - tileOffsetY,
                            spriteTiles,
                            0 * tileWidth,
                            1 * tileHeight,
                            tileWidth, tileHeight);
                        break;
                    case L'o':
                        Fill(
                            x * tileWidth - tileOffsetX,
                            y * tileHeight - tileOffsetY,
                            (x + 1) * tileWidth - tileOffsetX,
                            (y + 1) * tileHeight - tileOffsetY,
                            PIXEL_SOLID,
                            FG_CYAN
                        );
                        DrawPartialSprite(
                            x * tileWidth - tileOffsetX,
                            y * tileHeight - tileOffsetY,
                            spriteTiles, 
                            3 * tileWidth,
                            0 * tileHeight,
                            tileWidth, tileHeight);
                        break;
                    case L'?':
                        DrawPartialSprite(
                            x * tileWidth - tileOffsetX,
                            y * tileHeight - tileOffsetY,
                            spriteTiles,
                            1 * tileWidth,
                            1 * tileHeight,
                            tileWidth, tileHeight);
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
        DrawPartialSprite(
            (player.position.x - offsetX) * tileWidth,
            (player.position.y - offsetY) * tileWidth,
            spriteMan,
            sprX * tileWidth,
            sprY * tileHeight, 
            tileWidth, tileHeight);


        return true;
    }
};

int main()
{
    OneLoneCoder_Plataformer game;
    if (game.ConstructConsole(256, 240, 4, 4))
        game.Start();

    return 0;
}
