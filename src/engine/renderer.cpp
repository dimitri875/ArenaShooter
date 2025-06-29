#include "engine/renderer.h"
#include "utils/utils.h"
#include "structs.h"
#include <math.h>
#include <iostream>
#include <vector>
#include <unordered_set>

static Vector2 lightDir = v_normalize({-1.0f, 0.5f});
static std::unordered_set<int> visited;

Color getColor(const Vector2& lightDir, const Wall& w)
{
    Vector2 wallDir = {w.x1 - w.x0, w.y1 - w.y0};
    wallDir = v_normalize(wallDir);

    float dot = wallDir * lightDir;
    dot = (dot + 1) * 100.0f + 55;

    Color c = {(unsigned char)(dot), (unsigned char)(dot-5), (unsigned char)(dot-35), (unsigned char)255};
    return c;
}

void renderWall(Color* frameBuffer, const Sector& s, const Wall& w, const Player& p)
{
    for (int ray = 0; ray < N_RAYS; ray++)
    {
        float ao = -FOV/2 + ray * (FOV/N_RAYS);
        float ra = p.yaw + ao;

        Vector2 rayDirection = {cos(ra), sin(ra)};
        Vector2 rayOrigin = {p.x, p.y};
        Vector2 point1 = {w.x0, w.y0};
        Vector2 point2 = {w.x1, w.y1};

        float distance = lineSegmentRayIntersection(rayOrigin, rayDirection, point1, point2);

        if (distance == INFINITY)
            continue;
        
        float playerZ = p.height;

        float topZ = s.ceil - playerZ;
        float bottomZ = s.floor - playerZ;

        float screenTop = (HEIGHT / 2.0f) - (topZ * SCALE) / (distance * cos(ao));
        float screenBottom = (HEIGHT / 2.0f) - (bottomZ * SCALE) / (distance * cos(ao));

        Color c = getColor(lightDir, w);

        drawRect(frameBuffer, ray * L_WIDTH, screenTop, L_WIDTH, screenBottom-screenTop, c);
    }
}

void renderSector(Color* frameBuffer, int id, const std::vector<Sector>& sectors, const std::vector<Wall>& walls, const Player& p)
{
    if (visited.find(id) != visited.end())
        return;
    visited.insert(id);

    const Sector& s = sectors[id-1];
    for (int off = 0; off < s.numWalls; off++)
    {
        const Wall& w = walls[s.idx+off];

        if (!w.portal)
        {
            renderWall(frameBuffer, s, w, p);
        }
        else
        {
            renderSector(frameBuffer, w.portal, sectors, walls, p);
        }
    }
}

void renderMap(Color* frameBuffer, const std::vector<Sector>& sectors, const std::vector<Wall>& walls, const Player& p)
{
    // for (const Sector& s : sectors)
    // {
    //     for (int idx = s.idx; idx < s.numWalls; idx++)
    //     {
    //         const Wall& w = walls[idx];
    //     }
    // }
    visited.clear();
    renderSector(frameBuffer, 1, sectors, walls, p);

    renderMiniMap(frameBuffer, sectors, walls, p);
}

void renderMiniMap(Color* frameBuffer, const std::vector<Sector>& sectors, const std::vector<Wall>& walls, const Player& p)
{
    float minX = INFINITY, minY = INFINITY;
    for (const Wall& w : walls)
    {
        minX = fmin(fmin(w.x0, w.x1), minX);
        minY = fmin(fmin(w.y0, w.y1), minY);
    }

    float xo = 20.0f - minX;
    float yo = 20.0f - minY;

    for (const Wall& w : walls)
    {
        Color c = (w.portal) ? BLUE : RED;
        drawLine(frameBuffer, w.x0+xo, w.y0+yo, w.x1+xo, w.y1+yo, c);
    }

    float dx = cos(p.yaw)*5.0f, dy = sin(p.yaw)*5.0f;

    drawRect(frameBuffer, p.x-1+xo, p.y-1+yo, 2, 2, GREEN);
    drawLine(frameBuffer, p.x+xo, p.y+yo, p.x+dx+xo, p.y+dy+yo, GREEN);
}