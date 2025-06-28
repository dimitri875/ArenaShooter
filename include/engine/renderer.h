#pragma once

#include "raylib.h"
#include "settings.h"
#include "structs.h"
#include <vector>

void renderMap(Color* frameBuffer, const std::vector<Sector>& sectors, const std::vector<Wall>& walls, const Player& p);

void renderPlayer();

void renderOverlay();