#include "raylib.h"
#include "engine/engine.h"
#include "utils/utils.h"
#include "settings.h"
#include <math.h>
#include <iostream>

int main(int argc, char **argv)
{
    InitWindow(WIDTH, HEIGHT, "Game");
    SetTargetFPS(FPS);

    Color* frameBuffer = new Color[WIDTH * HEIGHT];
    Image frameImage;

    frameImage.data = frameBuffer;
    frameImage.width = WIDTH;
    frameImage.height = HEIGHT;
    frameImage.mipmaps = 1;
    frameImage.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;

    loadMap("test.txt");

    Texture2D frame = LoadTextureFromImage(frameImage);
    
    while (!WindowShouldClose())
    {
        renderFrame(frameBuffer);

        UpdateTexture(frame, frameBuffer);

        BeginDrawing();
            ClearBackground(BLACK);
            DrawTextureEx(frame, Vector2{0, 0}, 0, 1, WHITE);
        EndDrawing();
    }

    UnloadTexture(frame);
    UnloadImage(frameImage);
    CloseWindow();
    
    return 0;
}