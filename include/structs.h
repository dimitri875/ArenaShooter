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

// struct RayHitInfo
// {
//     float x, y, dist;
//     int sectorId, wallIdx;
//     int portal;
// };

struct RayHit
{
    int wallIdx, portal, frontSector, backSector;
    float distance;
};
