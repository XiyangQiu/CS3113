#include "CS3113/cs3113.h"

enum TeardropStatus { HANGING, RELEASED };
enum GameStatus { PLAYING, LEFT_WINS, RIGHT_WINS };
enum GameMode { PVP, PVE };

// Global Constants
constexpr int SCREEN_WIDTH  = 1000,
              SCREEN_HEIGHT = 650,
              FPS           = 60,
              SPEED         = 300,
              MAX_BULLETS   = 3;
              

constexpr Vector2 ORIGIN                = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 },
                  SHIELD_SIZE           = { 40.0f, 160.0f },                  
                  LEFT_SHIELD_INIT_POS  = { 120.0f, ORIGIN.y },
                  RIGHT_SHIELD_INIT_POS = { SCREEN_WIDTH - 120.0f, ORIGIN.y },
                  BACKGROUND_SIZE       = { SCREEN_WIDTH, SCREEN_HEIGHT },
                  COWBOY_SIZE           = { 100.0f, SCREEN_HEIGHT * 0.7f },
                  LEFT_COWBOY_INIT_POS  = { 50.0f, ORIGIN.y},
                  RIGHT_COWBOY_INIT_POS = { SCREEN_WIDTH - 50.0f, ORIGIN.y},
                  GUN_SIZE              = { 100.0f, 80.0f },
                  BULLET_SIZE           = { 20.0f, 10.0f },
                  WIN_SIZE              = { 400.0f, 200.0f };

// @see https://idol.st/idols/ for source
constexpr char SHIELD_FP[] = "assets/game/shield.png",
               BACKGROUND_FP[] = "assets/game/desert.png",
               LEFT_COWBOY_FP[] = "assets/game/cowboy.png",
               RIGHT_COWBOY_FP[] = "assets/game/cowboy_flip.png",
               GUN_FP[] = "assets/game/gun.png",
               BULLET_FP[] = "assets/game/bullet.png",
               AI_COWBOY_FP[] = "assets/game/ai_cowboy.png",
               LEFT_WINS_FP[] = "assets/game/left_win.png",
               RIGHT_WINS_FP[] = "assets/game/right_win.png";

constexpr float BULLET_SPEED = 400.0f,
                FIRE_INTERVAL = 2.0f;

// Global Variables
AppStatus gAppStatus     = RUNNING;
GameStatus gGameStatus   = PLAYING;
GameMode gGameMode     = PVE;
int       gBulletsToFire   = 0;
float     gAngle         = 0.0f,
          gPreviousTicks = 0.0f,
          gGunAngle      = 0.0f,
          gBulletAngle   = 0.0f,
          gFireTimer    = 0.0f;

Vector2 gLeftShieldPosition = LEFT_SHIELD_INIT_POS,
        gLeftShieldMovement = {0.0f, 0.0f},
        gLeftShieldScale    = SHIELD_SIZE,
        gRightShieldPosition = RIGHT_SHIELD_INIT_POS,
        gRightShieldMovement = {0.0f, 0.0f},
        gRightShieldScale    = SHIELD_SIZE,
        gLeftCowboyPosition  = LEFT_COWBOY_INIT_POS,
        gRightCowboyPosition = RIGHT_COWBOY_INIT_POS,
        gBulletPosition[MAX_BULLETS]      = {ORIGIN,ORIGIN,ORIGIN},
        gBulletMovement[MAX_BULLETS]      = {{0.0f, 0.0f},{0.0f, 0.0f},{0.0f, 0.0f}},
        gMousePosition = GetMousePosition();

bool gBulletIsActive[MAX_BULLETS] = {false, false, false};

Texture2D gTeardropTexture;
Texture2D gBeakerTexture;
Texture2D gLeftShieldTexture;
Texture2D gRightShieldTexture;
Texture2D gBackgroundTexture;
Texture2D gLeftCowboyTexture;
Texture2D gRightCowboyTexture;
Texture2D gGunTexture;
Texture2D gBulletTexture;
Texture2D gAICowboyTexture;
Texture2D gLeftWinsTexture;
Texture2D gRightWinsTexture;

TeardropStatus gTeardropStatus = HANGING;

// Function Declarations
void initialise();
void processInput();
void update();
void render();
void shutdown();
bool isColliding(const Vector2 *positionA, const Vector2 *scaleA, const Vector2 *positionB, const Vector2 *scaleB);

// Function Definitions

/**
 * @brief Checks for a square collision between 2 Rectangle objects.
 *
 * @param positionA The position of the first object
 * @param scaleA The scale of the first object
 * @param positionB The position of the second object
 * @param scaleB The scale of the second object
 * @return true if a collision is detected,
 * @return false if a collision is not detected
 */
void fireBullet(int index){
    gBulletPosition[index] = ORIGIN;
    int angle = GetRandomValue(-45, 45);
    if(GetRandomValue(0, 1) == 0){angle += 180;}
    gGunAngle = static_cast<float>(angle);
    gBulletAngle = gGunAngle;
    float radians = gBulletAngle * PI / 180.0f;
    gBulletMovement[index] = {cosf(radians), sinf(radians)};
    gBulletIsActive[index] = true;
}
bool isColliding(const Vector2 *positionA, const Vector2 *scaleA,
                 const Vector2 *positionB, const Vector2 *scaleB)
{
    float xDistance = fabs(positionA->x - positionB->x) -
                      ((scaleA->x + scaleB->x) / 2.0f);
    float yDistance = fabs(positionA->y - positionB->y) -
                      ((scaleA->y + scaleB->y) / 2.0f);

    return xDistance < 0.0f && yDistance < 0.0f;
}

void renderObject(const Texture2D *texture, const Vector2 *position,
                  const Vector2 *scale)
{
    // Whole texture (UV coordinates)
    Rectangle textureArea = {
        // top-left corner
        0.0f, 0.0f,

        // bottom-right corner (of texture)
        static_cast<float>(texture->width),
        static_cast<float>(texture->height)
    };

    // Destination rectangle – centred on gPosition
    Rectangle destinationArea = {
        position->x,
        position->y,
        static_cast<float>(scale->x),
        static_cast<float>(scale->y)
    };

    // Origin inside the source texture (centre of the texture)
    Vector2 originOffset = {
        static_cast<float>(scale->x) / 2.0f,
        static_cast<float>(scale->y) / 2.0f
    };

    // Render the texture on screen
    DrawTexturePro(
        *texture,
        textureArea, destinationArea, originOffset,
        gAngle, WHITE
    );
}

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Western Pong");

    gLeftShieldTexture  = LoadTexture(SHIELD_FP);
    gRightShieldTexture = LoadTexture(SHIELD_FP);
    gBackgroundTexture  = LoadTexture(BACKGROUND_FP);
    gLeftCowboyTexture  = LoadTexture(LEFT_COWBOY_FP);
    gRightCowboyTexture = LoadTexture(RIGHT_COWBOY_FP);
    gAICowboyTexture    = LoadTexture(AI_COWBOY_FP);
    gGunTexture         = LoadTexture(GUN_FP);
    gBulletTexture      = LoadTexture(BULLET_FP);
    gLeftWinsTexture    = LoadTexture(LEFT_WINS_FP);
    gRightWinsTexture   = LoadTexture(RIGHT_WINS_FP);

    SetTargetFPS(FPS);
}

void processInput()
{
    // to close the game
    if (IsKeyPressed(KEY_Q) || WindowShouldClose()){
        gAppStatus = TERMINATED;
        return;
    }

    if(gGameStatus != PLAYING){
        return;
    }
    
    gLeftShieldMovement  = { 0.0f, 0.0f };
    gRightShieldMovement = { 0.0f, 0.0f };

    if(gGameMode == PVP){
        if (IsKeyDown(KEY_W)){gLeftShieldMovement.y = -1.5f;}
        else if (IsKeyDown(KEY_S)){gLeftShieldMovement.y = 1.5f;}
    }
    if (IsKeyDown(KEY_UP)){gRightShieldMovement.y = -1.5f;}
    else if (IsKeyDown(KEY_DOWN)){gRightShieldMovement.y = 1.5f;}

    int requestedBullets = 0;
    if(IsKeyPressed(KEY_ONE)){requestedBullets = 1;}
    if(IsKeyPressed(KEY_TWO)){requestedBullets = 2;}
    if(IsKeyPressed(KEY_THREE)){requestedBullets = 3;}
    if(requestedBullets){
        gBulletsToFire = requestedBullets;
        for(int i = 0; i < MAX_BULLETS; i++){
            if(gBulletIsActive[i]){
                if(gBulletsToFire > 0){gBulletsToFire--;}
                else{gBulletIsActive[i] = false;}
            }
        }
        gFireTimer = FIRE_INTERVAL;
    }

    if(IsKeyPressed(KEY_T)){
        if(gGameMode == PVE){gGameMode = PVP;}
        else{gGameMode = PVE;}
    }
    
    
}

void update()
{
    // delta time
    float ticks = static_cast<float>(GetTime());
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks  = ticks;

    if(gGameStatus != PLAYING){
        return;
    }
    
    if(gBulletsToFire > 0){
        gFireTimer += deltaTime;
        if(gFireTimer >= FIRE_INTERVAL){
            for(int i = 0; i < MAX_BULLETS; i++){
                if(!gBulletIsActive[i]){
                    fireBullet(i);
                    gBulletsToFire--;
                    gFireTimer = 0.0f;
                    break;
                }
            }
        }
    }

    gLeftShieldPosition.y  += gLeftShieldMovement.y * SPEED * deltaTime;
    gRightShieldPosition.y += gRightShieldMovement.y * SPEED * deltaTime;

    if(gLeftShieldPosition.y < gLeftShieldScale.y / 2.0f)
        gLeftShieldPosition.y = gLeftShieldScale.y / 2.0f;
    else if(gLeftShieldPosition.y > SCREEN_HEIGHT - gLeftShieldScale.y / 2.0f)
        gLeftShieldPosition.y = SCREEN_HEIGHT - gLeftShieldScale.y / 2.0f;

    if(gRightShieldPosition.y < gRightShieldScale.y / 2.0f)
        gRightShieldPosition.y = gRightShieldScale.y / 2.0f;
    else if(gRightShieldPosition.y > SCREEN_HEIGHT - gRightShieldScale.y / 2.0f)
        gRightShieldPosition.y = SCREEN_HEIGHT - gRightShieldScale.y / 2.0f;

    for(int i = 0; i < MAX_BULLETS; i++){
        if(gBulletIsActive[i]){
            if(gGameMode == PVE){
                gLeftShieldPosition.y = gBulletPosition[i].y;
                if(gLeftShieldPosition.y < gLeftShieldScale.y / 2.0f)
                    gLeftShieldPosition.y = gLeftShieldScale.y / 2.0f;
                else if(gLeftShieldPosition.y > SCREEN_HEIGHT - gLeftShieldScale.y / 2.0f)
                    gLeftShieldPosition.y = SCREEN_HEIGHT - gLeftShieldScale.y / 2.0f;
            }
            gBulletPosition[i].x += gBulletMovement[i].x * BULLET_SPEED * deltaTime;
            gBulletPosition[i].y += gBulletMovement[i].y * BULLET_SPEED * deltaTime;
            if(gBulletPosition[i].x + BULLET_SIZE.x / 2.0f < 0.0f || gBulletPosition[i].x - BULLET_SIZE.x / 2.0f > SCREEN_WIDTH){
                gBulletIsActive[i] = false;
            }
            if(gBulletMovement[i].x < 0.0f && isColliding(&gBulletPosition[i], &BULLET_SIZE, &gLeftShieldPosition, &gLeftShieldScale)){
                gBulletMovement[i].x *= -1.0f;
                gBulletPosition[i].x = gLeftShieldPosition.x + gLeftShieldScale.x / 2.0f + BULLET_SIZE.x / 2.0f;
            }
            else if(gBulletMovement[i].x > 0.0f && isColliding(&gBulletPosition[i], &BULLET_SIZE, &gRightShieldPosition, &gRightShieldScale)){
                gBulletMovement[i].x *= -1.0f;
                gBulletPosition[i].x = gRightShieldPosition.x - gRightShieldScale.x / 2.0f - BULLET_SIZE.x / 2.0f;
            }
            if(isColliding(&gBulletPosition[i], &BULLET_SIZE, &gLeftCowboyPosition, &COWBOY_SIZE)){
                gGameStatus = RIGHT_WINS;
                gBulletIsActive[i] = false;
                return;
            }
            else if(isColliding(&gBulletPosition[i], &BULLET_SIZE, &gRightCowboyPosition, &COWBOY_SIZE)){
                gGameStatus = LEFT_WINS;
                gBulletIsActive[i] = false;
                return;
            }
            if(gBulletPosition[i].y - BULLET_SIZE.y / 2.0f < 0.0f || gBulletPosition[i].y + BULLET_SIZE.y / 2.0f > SCREEN_HEIGHT){
                gBulletMovement[i].y *= -1.0f;
            }
        }
    }
}

void render()
{
    BeginDrawing();
    ClearBackground(RAYWHITE);
    //render background
    renderObject(&gBackgroundTexture, &ORIGIN, &BACKGROUND_SIZE);
    // render shields
    renderObject(&gLeftShieldTexture, &gLeftShieldPosition, &gLeftShieldScale);
    renderObject(&gRightShieldTexture, &gRightShieldPosition, &gRightShieldScale);
    // render cowboys
    if(gGameStatus != RIGHT_WINS){
        if(gGameMode == PVE){renderObject(&gAICowboyTexture, &gLeftCowboyPosition, &COWBOY_SIZE);}
        else{renderObject(&gLeftCowboyTexture, &gLeftCowboyPosition, &COWBOY_SIZE);}
    }
    if(gGameStatus != LEFT_WINS){
        renderObject(&gRightCowboyTexture, &gRightCowboyPosition, &COWBOY_SIZE);
    }
    // render gun
    Rectangle gunTextureArea = {0.0f, 0.0f, static_cast<float>(gGunTexture.width), static_cast<float>(gGunTexture.height)};
    Rectangle gunDestinationArea = {ORIGIN.x, ORIGIN.y, GUN_SIZE.x, GUN_SIZE.y};
    Vector2 gunOriginOffset = {GUN_SIZE.x, GUN_SIZE.y / 5.0f};
    DrawTexturePro(gGunTexture, gunTextureArea, gunDestinationArea, gunOriginOffset, gGunAngle, WHITE);
    //render bullet
    for(int i = 0; i < MAX_BULLETS; i++){
        if(gBulletIsActive[i]){
            renderObject(&gBulletTexture, &gBulletPosition[i], &BULLET_SIZE);
        }
    }
    if(gGameStatus == LEFT_WINS){renderObject(&gLeftWinsTexture, &ORIGIN, &WIN_SIZE);}
    else if(gGameStatus == RIGHT_WINS){renderObject(&gRightWinsTexture, &ORIGIN, &WIN_SIZE);}
    EndDrawing();
}

void shutdown()
{
    UnloadTexture(gLeftShieldTexture);
    UnloadTexture(gRightShieldTexture);
    UnloadTexture(gBackgroundTexture);
    UnloadTexture(gLeftCowboyTexture);
    UnloadTexture(gRightCowboyTexture);
    UnloadTexture(gGunTexture);
    UnloadTexture(gBulletTexture);
    UnloadTexture(gAICowboyTexture);
    UnloadTexture(gLeftWinsTexture);
    UnloadTexture(gRightWinsTexture);
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
