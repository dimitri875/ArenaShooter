#include "player/player.h"
#include "structs.h"
#include "raylib.h"
#include <math.h>
#include <iostream>

void moveForwards(Player& p, const float velocity)
{
    Vector2 dir = {cos(p.yaw), sin(p.yaw)};
    p.x += velocity * dir.x;
    p.y += velocity * dir.y;
}

void moveBackwards(Player& p, const float velocity)
{
    Vector2 dir = {cos(p.yaw), sin(p.yaw)};
    p.x -= velocity * dir.x;
    p.y -= velocity * dir.y;
}

void strafeLeft(Player& p, const float velocity)
{
    Vector2 dir = {-sin(p.yaw), cos(p.yaw)};
    p.x -= velocity * dir.x;
    p.y -= velocity * dir.y;
}

void strafeRight(Player& p, const float velocity)
{
    Vector2 dir = {-sin(p.yaw), cos(p.yaw)};
    p.x += velocity * dir.x;
    p.y += velocity * dir.y;
}

void changeYaw(Player& p, const float angle)
{
    p.yaw += angle;
}

void changePitch(Player& p, const float angle)
{
    p.pitch += angle;
}