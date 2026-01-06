#include "engine/engine.h"
#include "engine/renderer.h"
#include "engine/map.h"
#include "engine/input.h"
#include "structs.h"
#include <vector>
#include <string>
#include <cassert>
#include <iostream>

std::vector<Sector> sectors;
std::vector<Wall> walls;
Player player = {0, 0, 3, 45.0f * PI / 180.0f, 0};

void loadMap(const std::string& path)
{
    loadLevelFromFile(path, sectors, walls);
}

void renderFrame(Color* frameBuffer)
{
    for (int i = 0; i < WIDTH * HEIGHT; i++) 
        frameBuffer[i] = BLACK;

    assert(sectors.size() > 0 && walls.size() > 0);
    renderMap(frameBuffer, sectors, walls, player);
    renderMiniMap(frameBuffer, sectors, walls, player);
}

void handleInput()
{
    handleKeyboard(player);
    handleMouse(player);
}