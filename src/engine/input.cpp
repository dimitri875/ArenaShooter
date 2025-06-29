#include "engine/input.h"
#include "player/player.h"
#include "structs.h"
#include "raylib.h"

void handleKeyboard(Player& p)
{
    const float velocity = 0.1f;
    const float angle = 0.05f;

    if (IsKeyDown(KEY_W))
    {
        moveForwards(p, velocity);
    }
    if (IsKeyDown(KEY_A))
    {
        strafeLeft(p, velocity);
    }
    if (IsKeyDown(KEY_S))
    {
        moveBackwards(p, velocity);
    }
    if (IsKeyDown(KEY_D))
    {
        strafeRight(p, velocity);
    }
    if (IsKeyDown(KEY_LEFT))
    {
        changeYaw(p, -angle);
    }
    if (IsKeyDown(KEY_RIGHT))
    {
        changeYaw(p, angle);
    }
    if (IsKeyDown(KEY_UP))
    {
        changePitch(p, angle);
    }
    if (IsKeyDown(KEY_DOWN))
    {
        changePitch(p, -angle);
    }
}

void handleMouse(Player& p)
{

}