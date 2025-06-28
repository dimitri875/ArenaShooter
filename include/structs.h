#pragma once

struct Sector
{
    int id, idx, numWalls;
    float floor, ceil;
};

struct Wall
{
    float x0, y0, x1, y1;
    int portal;
};

struct Player
{
    float x, y, height;
    float yaw, pitch;
};
