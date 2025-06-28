#include "engine/engine.h"
#include "engine/renderer.h"
#include "engine/map.h"
#include "structs.h"
#include <vector>
#include <string>
#include <cassert>

std::vector<Sector> sectors;
std::vector<Wall> walls;
Player player;

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
}