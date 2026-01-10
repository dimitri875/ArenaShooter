#include "engine/renderer.h"
#include "utils/utils.h"
#include "structs.h"
#include <math.h>
#include <iostream>
#include <vector>
#include <algorithm>

static Vector2 lightDir = v_normalize({-1.0f, 0.5f});
static std::vector<RayHit> rayInfo[N_RAYS];

Color getColor(const Vector2& lightDir, const Wall& w)
{
    Vector2 wallDir = {w.x1 - w.x0, w.y1 - w.y0};
    wallDir = v_normalize(wallDir);

    float dot = wallDir * lightDir;
    dot = (dot + 1) * 100.0f + 55;

    Color c = {(unsigned char)(dot), (unsigned char)(dot-5), (unsigned char)(dot-35), (unsigned char)255};
    return c;
}

std::vector<RayHit> getRayHitInfo(int ray, const Vector2& origin, const Vector2& direction, const std::vector<Sector>& sectors, const std::vector<Wall>& walls)
{
    std::vector<RayHit> info;

    for (const Sector& s : sectors)
    {
        for (int off = 0; off < s.numWalls; off++)
        {
            int wallIdx = s.idx+off;
            const Wall& w = walls[wallIdx];

            Vector2 point1 = {w.x0, w.y0};
            Vector2 point2 = {w.x1, w.y1};

            float distance = lineSegmentRayIntersection(origin, direction, point1, point2);

            if (distance != INFINITY) {
                RayHit hit = {wallIdx, distance};
                info.push_back(hit);
            }
        }

    }
    
    std::sort(info.begin(), info.end(), [](const RayHit &a, const RayHit &b) {
        return a.distance < b.distance;
    });
    
    return info;
}

void renderMap(Color* frameBuffer, const std::vector<Sector>& sectors, const std::vector<Wall>& walls, const Player& p)
{
    int sectorId = getPlayerSector(p, sectors, walls);
    std::cout << "Sector Id: " << sectorId << std::endl;
    if (sectorId == 0) return;

    Vector2 origin = {p.x, p.y};
    
    // Cast rays and store all wall hits per ray
    for (int ray = 0; ray < N_RAYS; ray++)
    {
        float ao = -FOV/2 + ray * FOV/N_RAYS;
        Vector2 direction = {cos(p.yaw + ao), sin(p.yaw + ao)};
        rayInfo[ray] = getRayHitInfo(ray, origin, direction, sectors, walls);
    }

    // Render each ray
    for (int ray = 0; ray < N_RAYS; ray++)
    {
        float ao = -FOV/2 + ray * FOV/N_RAYS;
        const std::vector<RayHit>& sortedHits = rayInfo[ray];

        int currentSectorId = sectorId;
        float playerZ = p.height;
        float width = (float)WIDTH / N_RAYS;
        float left = ray * width;

        // Track the top and bottom of the vertical slice that has already been drawn
        float currentTop = 0;
        float currentBottom = HEIGHT;

        for (const RayHit& hit : sortedHits)
        {
            const Wall& w = walls[hit.wallIdx];
            const Sector& currentSector = sectors[currentSectorId - 1];

            // 1. Project Current Sector Heights (The "Frame" we are looking through)
            float ceilZ = currentSector.ceil - playerZ;
            float floorZ = currentSector.floor - playerZ;
            
            // Note: Ensure hit.distance is not zero to avoid division by zero
            float dist = hit.distance * cos(ao); 
            float screenCeil = (HEIGHT / 2.0f) - (ceilZ * SCALE) / dist;
            float screenFloor = (HEIGHT / 2.0f) - (floorZ * SCALE) / dist;

            // Solid wall (no portal)
            if (!w.backSector)
            {
                // Clip the solid wall to the current view window
                float drawTop = fmax(currentTop, screenCeil);
                float drawBottom = fmin(currentBottom, screenFloor);

                if (drawBottom > drawTop) {
                    drawRect(frameBuffer, left, drawTop, width, drawBottom - drawTop, getColor(lightDir, w));
                }
                break; // Wall is solid, stop processing this ray
            }

            // Portal wall
            int nextSectorId = (currentSectorId == w.frontSector) ? w.backSector : w.frontSector;
            const Sector& nextSector = sectors[nextSectorId - 1];

            // 2. Project Next Sector Heights (The "Target" we are looking at)
            float nextCeilZ = nextSector.ceil - playerZ;
            float nextFloorZ = nextSector.floor - playerZ;
            float screenNextCeil = (HEIGHT / 2.0f) - (nextCeilZ * SCALE) / dist;
            float screenNextFloor = (HEIGHT / 2.0f) - (nextFloorZ * SCALE) / dist;

            // 3. Render Upper Wall (The drop from Current Ceiling to Next Ceiling)
            // Only visible if next ceiling is physically lower (Screen Y is higher)
            if (screenNextCeil > screenCeil) 
            {
                float drawTop = fmax(currentTop, screenCeil);       // Start at Current Ceiling (clamped)
                float drawBottom = fmin(currentBottom, screenNextCeil); // End at Next Ceiling (clamped)

                if (drawBottom > drawTop) {
                    drawRect(frameBuffer, left, drawTop, width, drawBottom - drawTop, RED);
                }
            }

            // 4. Render Lower Wall (The step up from Current Floor to Next Floor)
            // Only visible if next floor is physically higher (Screen Y is lower)
            if (screenNextFloor < screenFloor) 
            {
                float drawTop = fmax(currentTop, screenNextFloor);  // Start at Next Floor (clamped)
                float drawBottom = fmin(currentBottom, screenFloor);    // End at Current Floor (clamped)

                if (drawBottom > drawTop) {
                    drawRect(frameBuffer, left, drawTop, width, drawBottom - drawTop, GREEN);
                }
            }

            // 5. Update Occlusion Window
            // The view into the next sector is restricted by the geometry of BOTH sectors.
            // Visually, the "hole" is the intersection of the two openings.
            float portalScreenTop = fmax(screenCeil, screenNextCeil);     // The lower of the two ceilings (Higher Screen Y)
            float portalScreenBottom = fmin(screenFloor, screenNextFloor); // The higher of the two floors (Lower Screen Y)

            currentTop = fmax(currentTop, portalScreenTop);
            currentBottom = fmin(currentBottom, portalScreenBottom);

            // Optimization: If the window closes up, stop casting
            if (currentTop >= currentBottom) break;

            // Update current sector
            currentSectorId = nextSectorId;
        }
    }
}


// void renderMap(Color* frameBuffer, const std::vector<Sector>& sectors, const std::vector<Wall>& walls, const Player& p)
// {
//     int sectorId = getPlayerSector(p, sectors, walls);
//     std::cout << "Sector Id: " << sectorId << std::endl;
//     if (sectorId == 0) {
//         return;
//     }

//     Vector2 origin = {p.x, p.y};
//     for (int ray = 0; ray < N_RAYS; ray++)
//     {
//         float ao = -FOV/2 + ray * FOV/N_RAYS;
//         Vector2 direction = {cos(p.yaw+ao), sin(p.yaw+ao)};
//         rayInfo[ray] = getRayHitInfo(ray, origin, direction, sectors, walls);
//     }
    
//     for (int ray = 0; ray < N_RAYS; ray++)
//     {

//         float ao = -FOV/2 + ray * FOV/N_RAYS;

//         const std::vector<RayHit>& sortedHits = rayInfo[ray];
//         int currentSectorId = sectorId;  // Need to update this for the case when there are portal walls after portal walls
//         for (const RayHit& hit : sortedHits) {
//             float playerZ = p.height;
            
//             const Wall& w = walls[hit.wallIdx];
//             const Sector& currentSector = sectors[currentSectorId-1];
            
//             float ceilHeight = currentSector.ceil;
//             float floorHeight = currentSector.floor;
            
//             float topZ = ceilHeight - playerZ;
//             float bottomZ = floorHeight - playerZ;

//             float currentTop = (HEIGHT / 2.0f) - (topZ * SCALE) / (hit.distance * cos(ao));;
//             float currentBottom = (HEIGHT / 2.0f) - (bottomZ * SCALE) / (hit.distance * cos(ao));
//             // Solid wall
//             if (!w.backSector) {

//                 float width = (float)WIDTH/N_RAYS;
//                 float left = ray * width;

//                 float top = currentTop;
//                 float height = currentBottom - currentTop;

//                 drawRect(frameBuffer, left, top, width, height, getColor(lightDir, w));
//                 break;
//             } else {
//                 // Finding next sector
//                 int nextSectorId = (currentSectorId == w.frontSector) ? w.backSector : w.frontSector;

//                 // Finding clipping and drawing
//                 float playerZ = p.height;
                
//                 float width = (float)WIDTH/N_RAYS;
//                 float left = ray * width;

//                 const Sector& currentSector = sectors[currentSectorId-1];
//                 const Sector& nextSector = sectors[nextSectorId-1];
//                 float floorHeight = fmin(currentSector.floor, nextSector.floor);
//                 float ceilHeight = fmax(currentSector.ceil, nextSector.ceil);
                
//                 float topZ = ceilHeight - playerZ;
//                 float bottomZ = floorHeight - playerZ;

//                 float nextTop = (HEIGHT / 2.0f) - (topZ * SCALE) / (hit.distance * cos(ao));
//                 float nextBottom = (HEIGHT / 2.0f) - (bottomZ * SCALE) / (hit.distance * cos(ao));

//                 if (currentTop < nextTop) {
//                     drawRect(frameBuffer, left, currentTop, width, nextTop-currentTop, RED);
//                     currentTop = nextTop;
//                 }

//                 if (currentBottom > nextBottom) {
//                     drawRect(frameBuffer, left, nextBottom, width, currentBottom-nextBottom, GREEN);
//                     currentBottom = nextBottom;
//                 }

//                 // Updating Current Sector
//                 currentSectorId = nextSectorId;
//             }
//         }
//     }
// }

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
        Color c = (w.backSector) ? BLUE : RED;
        drawLine(frameBuffer, w.x0+xo, w.y0+yo, w.x1+xo, w.y1+yo, c);
    }

    float dx = cos(p.yaw)*5.0f, dy = sin(p.yaw)*5.0f;

    drawRect(frameBuffer, p.x-1+xo, p.y-1+yo, 2, 2, GREEN);
    drawLine(frameBuffer, p.x+xo, p.y+yo, p.x+dx+xo, p.y+dy+yo, GREEN);
}

int getPlayerSector(const Player& p, const std::vector<Sector> &sectors, const std::vector<Wall> &walls)
{
    for (const Sector& s : sectors)
    {
        if (pointInPolygon(p.x, p.y, walls, s.idx, s.numWalls)) {
            return s.id;
        }
    }
    return 0;
}