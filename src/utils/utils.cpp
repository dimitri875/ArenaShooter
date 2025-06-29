#include "utils/utils.h"
#include "settings.h"
#include <math.h>

Vector2 operator*(const Vector2& v, float s)
{
    return {v.x * s, v.y * s};
}

Vector2 operator*(float s, const Vector2& v)
{
    return v * s;
}

float operator*(const Vector2& v, const Vector2& u)
{
    return (v.x * u.x + v.y * u.y);
}

Vector2 operator+(const Vector2& a, const Vector2& b)
{
    return {a.x + b.x, a.y + b.y};
}

Vector2 operator-(const Vector2& a, const Vector2& b)
{
    return {a.x - b.x, a.y - b.y};
}

float v_getMagnitude(const Vector2& v)
{
    return sqrt(pow(v.x, 2) + pow(v.y, 2));
}

Vector2 v_normalize(const Vector2& v)
{
    return {v.x / v_getMagnitude(v), v.y / v_getMagnitude(v)};
}

Vector2 v_invert(const Vector2& v)
{
    return {-1 * v.x, -1 * v.y};
}

float cross(const Vector2& a, const Vector2& b)
{
    return (a.x * b.y - a.y * b.x);
}

void drawLine(Color* frameBuffer, float x0, float y0, float x1, float y1, Color c)
{
    float dx = x1 - x0;
    float dy = y1 - y0;

    int steps = (int)(fmax(fabs(dx), fabs(dy)));

    if (steps == 0) return;

    float xInc = dx / steps;
    float yInc = dy / steps;

    float x = x0;
    float y = y0;

    for (int i = 0; i <= steps; i++)
    {
        int ix = (int)x;
        int iy = (int)y;

        if (ix >= 0 && ix < WIDTH && iy >= 0 && iy < HEIGHT)
        {
            frameBuffer[ix + iy * WIDTH] = c;
        }

        x += xInc;
        y += yInc;
    }
}

void drawRect(Color* frameBuffer, float left, float top, float width, float height, Color c)
{
    int ileft   = (int)left;
    int itop    = (int)top;
    int iwidth  = (int)width;
    int iheight = (int)height;

    for (int w = 0; w < iwidth; w++)
    {
        drawLine(frameBuffer, ileft+w, itop, ileft+w, itop+iheight, c);
    }
}

float lineSegmentRayIntersection(const Vector2& rayOrigin, const Vector2& rayDirection, const Vector2& point1, const Vector2& point2)
{
    Vector2 v1 = rayOrigin - point1;
    Vector2 v2 = point2 - point1;
    Vector2 v3 = {-rayDirection.y, rayDirection.x};

    float dot = v2 * v3;

    if (fabs(dot) < 1e-6)
        return INFINITY;
    
    float t1 = cross(v2, v1) / dot;
    float t2 = (v1 * v3) / dot;

    if (t1 >= 0.0f && t2 >= 0.0f && t2 <= 1.0)
        return t1;
    
    return INFINITY;
}