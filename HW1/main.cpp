/**
* Author: Xiyang Qiu
* Assignment: Simple 2D Scene
* Date due: 10/5/2026
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/
#include "CS3113/cs3113.h"
#include <math.h>

// Global Constants
constexpr int   SCREEN_WIDTH  = 2000,
                SCREEN_HEIGHT = 1200,
                FPS           = 60,
                SUN_SIZE          = 200,
                EARTH_SIZE        = 100,
                MOON_SIZE         = 50;
constexpr float MAX_AMP       = 5.0f,
                SUN_PATH_SIDE = 20.0f,
                SUN_SPEED     = 2.0f;

char    BG_COLOUR[] = "#ebb811";
constexpr Vector2 ORIGIN      = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
constexpr Vector2 BASE_SIZE   = { static_cast<float>(SUN_SIZE), static_cast<float>(SUN_SIZE) };

// Image owned by Gorillaz @see https://gorillaz.com/
constexpr char SUN_FP[] = "assets/sun.png";
constexpr char EARTH_FP[] = "assets/earth.png";
constexpr char MOON_FP[] = "assets/moon.png";

// Global Variables
AppStatus gAppStatus     = RUNNING;
float     gSunScaleFactor   = SUN_SIZE,
          gEarthAngle    = 0.0f,
          gPulseTime     = 0.0f;
Vector2   gSunPosition      = ORIGIN;
Vector2   gEarthPosition      = ORIGIN;
Vector2   gMoonPosition      = ORIGIN;
Vector2   gSunScale         = BASE_SIZE;
float     gPreviousTicks = 0.0f;

Texture2D gSunTexture;
Texture2D gEarthTexture;
Texture2D gMoonTexture;

// Function Declarations
void initialise();
void processInput();
void update();
void render();
void shutdown();

// Function Definitions
void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Project 1");

    gSunTexture = LoadTexture(SUN_FP);
    gEarthTexture = LoadTexture(EARTH_FP);
    gMoonTexture = LoadTexture(MOON_FP);

    SetTargetFPS(FPS);
}

void processInput()
{
    if (WindowShouldClose()) gAppStatus = TERMINATED;
}

void update()
{
    /**
     * @todo Calculate delta time
     */
    float ticks = static_cast<float>(GetTime());
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks = ticks;
// delta time
// speed mode
    gPulseTime += 0.5f * deltaTime;
// real life mode
    // gPulseTime += 1.0f/24.0f/3600.0f * deltaTime;
// sun position
    float distance = gPulseTime * SUN_SPEED;
    while (distance >= 4.0f * SUN_PATH_SIDE)
    {
        distance -= 4.0f * SUN_PATH_SIDE;
    }

    float left = ORIGIN.x - SUN_PATH_SIDE / 2.0f;
    float top  = ORIGIN.y - SUN_PATH_SIDE / 2.0f;
    if(distance < SUN_PATH_SIDE){
        gSunPosition = {left + distance, top};
    }
    else if (distance < 2.0f * SUN_PATH_SIDE){
        gSunPosition = {
            left + SUN_PATH_SIDE,
            top + distance - SUN_PATH_SIDE
        };
    }
    else if (distance < 3.0f * SUN_PATH_SIDE){
        gSunPosition = {
            left + 3.0f * SUN_PATH_SIDE - distance,
            top + SUN_PATH_SIDE
        };
    }
    else{
        gSunPosition = {
            left,
            top + 4.0f * SUN_PATH_SIDE - distance
        };
    }
    gSunScale = {
        BASE_SIZE.x + MAX_AMP * cos(gPulseTime),
        BASE_SIZE.y + MAX_AMP * cos(gPulseTime)
    };

// earth position relative to sun
    gEarthPosition.x = gSunPosition.x
                    + 800.0f * cos(2.0f * PI * gPulseTime / 365.0f);
    gEarthPosition.y = gSunPosition.y
                    + 400.0f * sin(2.0f * PI * gPulseTime / 365.0f);

    gEarthAngle = gPulseTime * 360.0f;
    while (gEarthAngle >= 360.0f)
    {
        gEarthAngle -= 360.0f;
    }
// moom posion relative to earth
    gMoonPosition.x = gEarthPosition.x
                   + 150.0f * cos(2.0f * PI * gPulseTime / 30.0f);
    gMoonPosition.y = gEarthPosition.y
                   + 150.0f * sin(2.0f * PI * gPulseTime / 30.0f);
}

void render()
{
    BeginDrawing();
    // ClearBackground(ColorFromHex(BG_COLOUR));
// background color( how the person on the erth sees the sun)
    // if (gEarthAngle < 45.0f){
    //     ClearBackground(ColorFromHex(BG_COLOUR));
    // }
    // else if (gEarthAngle < 135.0f){
    //     ClearBackground(RAYWHITE);
    // }
    // else if (gEarthAngle < 225.0f){
    //     ClearBackground(ColorFromHex(BG_COLOUR));
    // }
    // else{
    //     ClearBackground(DARKBLUE);
    // }

    float relativePositionSToE_X = gSunPosition.x - gEarthPosition.x;
    float relativePositionSToE_Y = gSunPosition.y - gEarthPosition.y;
    float relativeAngle = atan2(relativePositionSToE_Y, relativePositionSToE_X) * 180.0f / PI +90.0f;
    float angleDifference = relativeAngle - gEarthAngle;
    while (angleDifference < -180.0f) angleDifference += 360.0f;
    while (angleDifference > 180.0f) angleDifference -= 360.0f;
    if (angleDifference > -45.0f && angleDifference < 45.0f){
        ClearBackground(RAYWHITE);
    }
    else if (angleDifference > 135.0f || angleDifference < -135.0f){
        ClearBackground(DARKBLUE);
    }
    else{
        ClearBackground(ColorFromHex(BG_COLOUR));
    }
    // printf("angleDifference: %f\n", angleDifference);
    // printf("relativeAngle: %f\n", relativeAngle);
    // printf("gEarthAngle: %f\n", gEarthAngle);
    /**
     * @todo Design your UV coordinates (i.e. textureArea) so that only one
     * member is being rendered onto the screen.
     */
// sun
    Rectangle sunTextureArea = {
        // top-left corner
        0.0f, 0.0f,

        // bottom-right corner (of texture)
        static_cast<float>(gSunTexture.width),
        static_cast<float>(gSunTexture.height)
    };

    // Destination rectangle – centred on gPosition
    Rectangle sunDestinationArea = {
        gSunPosition.x,
        gSunPosition.y,
        static_cast<float>(gSunScale.x),
        static_cast<float>(gSunScale.y)
    };

    // Origin inside the source texture (centre of the texture)
    Vector2 sunOrigin = {
        static_cast<float>(gSunScale.x) / 2.0f,
        static_cast<float>(gSunScale.y) / 2.0f
    };

    // Render the texture on screen
    DrawTexturePro(
        gSunTexture,
        sunTextureArea,
        sunDestinationArea,
        sunOrigin,
        0.0f,
        WHITE
    );
// earth
    Rectangle earthTextureArea = {
        // top-left corner
        0.0f, 0.0f,

        // bottom-right corner (of texture)
        static_cast<float>(gEarthTexture.width),
        static_cast<float>(gEarthTexture.height)
    };

    // Destination rectangle – centred on gPosition
    Rectangle earthDestinationArea = {
        gEarthPosition.x,
        gEarthPosition.y,
        EARTH_SIZE,
        EARTH_SIZE
    };

    // Origin inside the source texture (centre of the texture)
    Vector2 earthOrigin = {
        static_cast<float>(EARTH_SIZE) / 2.0f,
        static_cast<float>(EARTH_SIZE) / 2.0f
    };

    // Render the texture on screen
    DrawTexturePro(
        gEarthTexture,
        earthTextureArea,
        earthDestinationArea,
        earthOrigin,
        gEarthAngle,
        WHITE
    );
// moon
    Rectangle moonTextureArea = {
        // top-left corner
        0.0f, 0.0f,

        // bottom-right corner (of texture)
        static_cast<float>(gMoonTexture.width),
        static_cast<float>(gMoonTexture.height)
    };

    // Destination rectangle – centred on gPosition
    Rectangle moonDestinationArea = {
        gMoonPosition.x,
        gMoonPosition.y,
        MOON_SIZE,
        MOON_SIZE
    };

    // Origin inside the source texture (centre of the texture)
    Vector2 moonOrigin = {
        static_cast<float>(MOON_SIZE) / 2.0f,
        static_cast<float>(MOON_SIZE) / 2.0f
    };

    // Render the texture on screen
    DrawTexturePro(
        gMoonTexture,
        moonTextureArea,
        moonDestinationArea,
        moonOrigin,
        0.0f,
        WHITE
    );


    EndDrawing();
}

void shutdown()
{
    UnloadTexture(gSunTexture);
    UnloadTexture(gEarthTexture);
    UnloadTexture(gMoonTexture);
    CloseWindow();
}

int main(void)
{
    initialise();

    while (gAppStatus == RUNNING)
    {
        processInput();
        update();
        render();
    }

    shutdown();

    return 0;
}