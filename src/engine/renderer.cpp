#include "engine/renderer.h"
#include "utils/utils.h"
#include "structs.h"
#include <math.h>
#include <iostream>
#include <vector>

float height = 500;
Wall test = {5, -2, 5, 2, 0};
Player p1 = {0, 0, 300, 0, 0};

void renderMap(Color* frameBuffer, const std::vector<Sector>& sectors, const std::vector<Wall>& walls, const Player& p)
{
    float xo = 50, yo = 100;
    drawLine(frameBuffer, test.x0+xo, test.y0+yo, test.x1+xo, test.y1+yo, RED);

    frameBuffer[(int)p1.x + (int)p1.y * WIDTH] = GREEN;

    for (int ray = 0; ray < N_RAYS; ray++)
    {
        float ao = -FOV/2 + ray * (FOV/N_RAYS);
        float ra = p1.yaw + ao;
        Vector2 rayDirection = {cos(ra), sin(ra)};
        Vector2 rayOrigin = {p1.x, p1.y};
        Vector2 point1 = {test.x0, test.y0};
        Vector2 point2 = {test.x1, test.y1};
        float distance = lineSegmentRayIntersection(rayOrigin, rayDirection, point1, point2);

        if (distance == INFINITY)
            continue;

        // std::cout << "Ray " << ray << " hit at distance: " << distance << "\n";
        
        float targetHeight = height/distance;
        float yCenter = HEIGHT/2.0f;

        drawLine(frameBuffer, ray, yCenter + targetHeight/2, ray, yCenter - targetHeight/2, GREEN);

        float tx = distance * cos(ra), ty = distance * sin(ra);

        drawLine(frameBuffer, p1.x+xo, p1.y+yo, tx+xo, ty+yo, YELLOW);
    }

    if (IsKeyDown(KEY_LEFT))
        p1.yaw -= 0.01f;
    if (IsKeyDown(KEY_RIGHT))
        p1.yaw += 0.01f;
}
