#pragma once

struct Sector
{
    int id, idx, numWalls;
    float floor, ceil;
};

struct Wall
{
    float x0, y0, x1, y1;
    int frontSector;  // The sector this wall belongs to
    int backSector;   // The sector this wall connects; 0 -> solid wall
};

struct Player
{
    float x, y, height;
    float yaw, pitch;
};

struct RayHit
{
    int wallIdx;
    float distance;
};
