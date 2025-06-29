#pragma once

#include "structs.h"

void moveForwards(Player& p, const float velocity);

void moveBackwards(Player& p, const float velocity);

void strafeLeft(Player& p, const float velocity);

void strafeRight(Player& p, const float velocity);

void changeYaw(Player& p, const float angle);

void changePitch(Player& p, const float angle);
