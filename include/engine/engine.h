#pragma once

#include "raylib.h"
#include "settings.h"
#include "structs.h"
#include <string>

void loadMap(const std::string& path);

void renderFrame(Color* frameBuffer);

void updateFrame();

void handleInput();