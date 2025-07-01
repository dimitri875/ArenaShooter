#include "engine/renderer.h"
#include "utils/utils.h"
#include "structs.h"
#include <math.h>
#include <iostream>
#include <vector>
#include <unordered_set>

static Vector2 lightDir = v_normalize({-1.0f, 0.5f});
static std::unordered_set<int> visited;
static RayHitInfo rayInfo[N_RAYS];
static std::unordered_set<int> wallSequence[N_RAYS];

Color getColor(const Vector2& lightDir, const Wall& w)
{
    Vector2 wallDir = {w.x1 - w.x0, w.y1 - w.y0};
    wallDir = v_normalize(wallDir);

    float dot = wallDir * lightDir;
    dot = (dot + 1) * 100.0f + 55;

    Color c = {(unsigned char)(dot), (unsigned char)(dot-5), (unsigned char)(dot-35), (unsigned char)255};
    return c;
}

RayHitInfo getRayHitInfo(int ray, const Vector2& origin, const Vector2& direction, const std::vector<Sector>& sectors, const std::vector<Wall>& walls)
{
    RayHitInfo info;
    info.dist = INFINITY;

    wallSequence[ray].clear();

    for (const Sector& s : sectors)
    {
        for (int off = 0; off < s.numWalls; off++)
        {
            const Wall& w = walls[s.idx+off];

            Vector2 point1 = {w.x0, w.y0};
            Vector2 point2 = {w.x1, w.y1};

            float distance = lineSegmentRayIntersection(origin, direction, point1, point2);

            if (distance != INFINITY)
            {
                wallSequence[ray].insert(s.idx+off);
            }

            if (distance < info.dist)
            {
                info.dist = distance;
                info.portal = w.portal;
                info.sectorId = s.id;
                info.wallIdx = s.idx+off;
            }
        }
    }

    return info;
}

void extendRayAndRender(Color* frameBuffer, int ray, int sectorId, const std::vector<Sector>& sectors, const std::vector<Wall>& walls, const Player& p)
{
    if (wallSequence[ray].size() == 0)
        return;

    float ao = -FOV/2 + ray * FOV/N_RAYS;
    int closestWallIdx = -1;
    float distance = INFINITY;

    for (int wIdx : wallSequence[ray])
    {
        const Wall& w = walls[wIdx];

        Vector2 origin = {p.x, p.y};
        Vector2 direction = {cos(p.yaw+ao), sin(p.yaw+ao)};
        Vector2 point1 = {w.x0, w.y0};
        Vector2 point2 = {w.x1, w.y1};

        float tmp = lineSegmentRayIntersection(origin, direction, point1, point2);

        if (tmp < distance)
        {
            distance = tmp;
            closestWallIdx = wIdx;
        }
    }

    if (closestWallIdx == -1)
        return;

    const Wall& w = walls[closestWallIdx];

    if (w.portal)
    {
        wallSequence[ray].erase(closestWallIdx);
        extendRayAndRender(frameBuffer, ray, w.portal, sectors, walls, p);
    }
    else
    {
        const Sector& s = sectors[sectorId];

        float playerZ = p.height;
        
        float topZ = s.ceil - playerZ;
        float bottomZ = s.floor - playerZ;
        
        float screenTop = (HEIGHT / 2.0f) - (topZ * SCALE) / (distance * cos(ao));
        float screenBottom = (HEIGHT / 2.0f) - (bottomZ * SCALE) / (distance * cos(ao));

        float left, top, width, height;
        width = (float)WIDTH/N_RAYS;
        left = ray * width;
        top = screenTop;
        height = screenBottom-screenTop;

        drawRect(frameBuffer, left, top, width, height, getColor(lightDir, w));
    }

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
    // visited.clear();
    // renderSector(frameBuffer, 1, sectors, walls, p);

    Vector2 origin = {p.x, p.y};
    for (int ray = 0; ray < N_RAYS; ray++)
    {
        float ao = -FOV/2 + ray * FOV/N_RAYS;
        Vector2 direction = {cos(p.yaw+ao), sin(p.yaw+ao)};
        rayInfo[ray] = getRayHitInfo(ray, origin, direction, sectors, walls);
    }

    for (int ray = 0; ray < N_RAYS; ray++)
    {
        float ao = -FOV/2 + ray * FOV/N_RAYS;

        const RayHitInfo& info = rayInfo[ray];
        const Wall& w = walls[info.wallIdx];
        const Sector& s = sectors[info.sectorId-1];

        float playerZ = p.height;
        
        float topZ = s.ceil - playerZ;
        float bottomZ = s.floor - playerZ;
        
        float screenTop = (HEIGHT / 2.0f) - (topZ * SCALE) / (info.dist * cos(ao));
        float screenBottom = (HEIGHT / 2.0f) - (bottomZ * SCALE) / (info.dist * cos(ao));

        float left, top, width, height;
        width = (float)WIDTH/N_RAYS;
        left = ray * width;
        top = screenTop;
        height = screenBottom - screenTop;

        if (info.portal)
        {
            const Sector& next = sectors[info.portal-1];
            
            float portalTopZ = next.ceil - playerZ;
            float portalBottomZ = next.floor - playerZ;

            float portalScreenTop = (HEIGHT / 2.0f) - (portalTopZ * SCALE) / (info.dist * cos(ao));
            float portalScreenBottom = (HEIGHT / 2.0f) - (portalBottomZ * SCALE) / (info.dist * cos(ao));

            if (portalScreenTop > screenTop)
            {
                drawRect(frameBuffer, left, top, width, portalScreenTop-screenTop, GREEN);
            }
            if (portalScreenBottom < screenBottom)
            {
                drawRect(frameBuffer, left, portalScreenBottom, width, portalScreenBottom-screenBottom, GREEN);
            }

            extendRayAndRender(frameBuffer, ray, next.id, sectors, walls, p);
        }
        else
        {
            drawRect(frameBuffer, left, top, width, height, getColor(lightDir, w));
        }
    }

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
