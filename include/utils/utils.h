#pragma once

#include "raylib.h"

Vector2 operator*(const Vector2& v, float s);

Vector2 operator*(float s, const Vector2& v);

float operator*(const Vector2& v, const Vector2& u);

Vector2 operator+(const Vector2& a, const Vector2& b);

Vector2 operator-(const Vector2& a, const Vector2& b);

Vector2 v_invert(const Vector2& v);

float v_getMagnitude(const Vector2& v);

Vector2 v_normalize(const Vector2& v);

float cross(const Vector2& a, const Vector2& b);

void drawLine(Color* frameBuffer, float x0, float y0, float x1, float y1, Color c);

void drawRect(Color* frameBuffer, float left, float top, float width, float height, Color c);

float lineSegmentRayIntersection(const Vector2& rayOrigin, const Vector2& rayDirection, const Vector2& point1, const Vector2& point2);