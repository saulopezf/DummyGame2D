/*
Bugs durante el desarrollo:
    1.
        Problema:
            Al iniciar el juego habia una franja negra en el top de la pantalla y la camara no se movia en el eje Y.

        Explicacion:
            Cuando inicializaba visibleTilesY, estaba calculando las tiles visibles con el ancho de la pantalla y no el alto,
            por lo que hacia que el eje Y del juego tuviese comportamientos extra�os (los citados en el problema del bug#1)

        Solucion:
            Solamente tuve que cambiar ScreenWidth() por ScreenHeight(), esto pasa por copiar y pegar lineas muy parecidas.

                X int visibleTilesY = ScreenWidth() / tileHeight;
                V int visibleTilesY = ScreenHeight() / tileHeight;

*/

#include <iostream>
using namespace std;

#include "olcConsoleGameEngine.h"

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

    // Player properties
    float playerPosX = 0.0f;
    float playerPosY = 0.0f;

    float playerVelX = 0.0f;
    float playerVelY = 0.0f;

protected:
    virtual bool OnUserCreate() {
        levelWidth = 64;
        levelHeight = 16;
        level += L"................................................................";
        level += L"................................................................";
        level += L"................................................................";
        level += L"................................................................";
        level += L".......................########.................................";
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

        // Input
        playerVelX = 0;
        playerVelY = 0;

        if (IsFocused()) {
            // UP
            if (GetKey(0x57).bHeld) {
                playerVelY = -6.0f;
            }

            // DOWN
            if (GetKey(0x53).bHeld) {
                playerVelY = 6.0f;
            }

            // LEFT
            if (GetKey(0x41).bHeld) {
                playerVelX = -6.0f;
            }

            // RIGHT
            if (GetKey(0x44).bHeld) {
                playerVelX = 6.0f;
            }
        }

        playerPosX += playerVelX * elapsedTime;
        playerPosY += playerVelY * elapsedTime;

        cameraPosX = playerPosX;
        cameraPosY = playerPosY;

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

        // Dibujar el mapa
        for (int x = 0; x < visibleTilesX; x++) {
            for (int y = 0; y < visibleTilesY; y++) {
                wchar_t tileId = getTile(x + offsetX, y + offsetY);

                switch (tileId) {
                    case L'.':
                        Fill(
                            x * tileWidth,
                            y * tileHeight,
                            (x + 1) * tileWidth,
                            (y + 1) * tileHeight,
                            PIXEL_SOLID,
                            FG_CYAN
                        );
                        break;
                    case L'#':
                        Fill(
                            x * tileWidth,
                            y * tileHeight,
                            (x + 1) * tileWidth,
                            (y + 1) * tileHeight,
                            PIXEL_SOLID,
                            FG_RED
                        );
                        break;
                    default:
                        Fill(
                            x * tileWidth,
                            y * tileHeight,
                            (x + 1) * tileWidth,
                            (y + 1) * tileHeight,
                            PIXEL_SOLID,
                            FG_BLACK
                        );
                        break;
                }
            }
        }
        //Fill(playerPosX * tileWidth, playerPosY * tileHeight, (playerPosX + 1) * tileWidth, (playerPosY + 1) * tileHeight, PIXEL_SOLID, FG_GREEN);
        Fill(
            (playerPosX - offsetX) * tileWidth,
            (playerPosY - offsetY) * tileWidth,
            (playerPosX - offsetX + 1.0f) * tileHeight,
            (playerPosY - offsetY + 1.0f) * tileHeight,
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