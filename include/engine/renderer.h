#pragma once

#include "raylib.h"
#include "settings.h"
#include "structs.h"
#include <vector>

void renderWall(Color* frameBuffer, const Sector& s, const Wall& w, const Player& p);

void renderSector(Color* frameBuffer, int id, const std::vector<Sector>& sectors, const std::vector<Wall>& walls, const Player& p);

void renderMap(Color* frameBuffer, const std::vector<Sector>& sectors, const std::vector<Wall>& walls, const Player& p);

void renderMiniMap(Color* frameBuffer, const std::vector<Sector>& sectors, const std::vector<Wall>& walls, const Player& p);

void renderPlayer();

void renderOverlay();