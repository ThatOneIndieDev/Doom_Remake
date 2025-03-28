#include <raylib.h> // This is the raylib library we will use for graphics generation and game physics.
#include <raymath.h> // Provides extra math features (such as complex vector manipulation operations) for game making. 
#include <stdio.h> 
#include <stdlib.h>
#include <math.h>
#include "rlgl.h"

// Global Variables
#define MAX_BULLETS 30 // Define the magazine size of the gun.

// These are the global variables for checking if a bullet hits an enemy.
bool gameOver = false;
bool enemyHit = false;
float respawnTimer = 0.0f;
Vector3 enemy_one_position = { 5.0f, 0.5f, 1.0f }; // Initial enemy position
float enemyRadius = 0.5f; // Radius of the enemy
int score = 0;           // Player's score
float elapsedTime = 0.0f; // Time tracker

// Bullet structure is made in order to set physics and generation for the bullet shooting feature for our code.
typedef struct Bullet { 
    Vector3 position; // Current position of our bullet. Which is a vector3 type (vector3 types in C are used for representing a object with a vector in 3D space)
    Vector3 direction; // Current direction of the bullet.
    //Vector3 velocity;
    bool active; // A flag to identify if the bullet is active/(in motion) or not.
} Bullet; // Bullet is a type alias

// Function prototypes
void Shoot(Bullet bullets[], int *bulletCount, Camera camera); // Handles the game's shooting mechanics. Has 3 parameters a bullet array (the magazine of the gun), the bullet count which will be made to track the current number of bullets and the camera in order to see where the bullet goes with respect to the player.
void UpdateBullets(Bullet bullets[], int bulletCount, float delta); // Updates the bullet positions. With a parameter as delta which is the time.
void DrawBullets(Bullet bullets[], int bulletCount); // Renders the bullet on the screen.
int titleScreen(void); // The iniital scene of the game the player will see upon entering the game.
int start_Game(void); //The main scene in the game (Where all the magic will happen).
int credits(void); //The final credits scene of the game.
//The following 2 function prototypes are for making and displaying the enemies inside the game to render them and for them to follow the first person around.
void DrawCubeTexture(Texture2D texture, Vector3 position, float width, float height, float length, Color color); // Draw cube textured
void DrawCubeTextureRec(Texture2D texture, Rectangle source, Vector3 position, float width, float height, float length, Color color);
//The following functions is to check if a bullet collides with an enemy.
int CheckCollisionAndShooting(Bullet bullets[], int bulletCount, Camera camera, Vector3 *enemyPosition);
bool CheckCollisionBulletEnemy(Vector3 bulletPos, Vector3 enemyPos, float enemyRadius);
// bool CheckCollisionBulletEnemy(Vector3 bulletPos, Vector3 prevBulletPos, Vector3 enemyPos, float bulletRadius, float enemyRadius);

int main() {
    InitAudioDevice(); // Initializes the audio system for the game to use.
    titleScreen(); // Launches the main game starting from the title screen  then to start game then to credits.
    CloseAudioDevice(); // Shuts down the audio system once the game finishes.
    CloseWindow(); // Closes the game window.
    return 0;
}

//------------------------------------------------------------------------------------
// Title Screen main entry point
//------------------------------------------------------------------------------------
int titleScreen(void) {
    const int screenWidth = 1600; // Setting a width for the game system. On display this shall change to 1920.
    const int screenHeight = 900; // Setting a height for the game system. On display this shall change to 1080.
    InitWindow(screenWidth, screenHeight, "Welcome to DOOM!"); // Initializing a window for the game to run in. 
    EnableCursor();

    Camera camera = { 0 }; // An array of type camera so that we can set and track the player's movements.
    camera.position = (Vector3){ 16.0f, 14.0f, 16.0f }; // The initial position of the camera in 3D space.
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f }; // This is basically where the camera is looking at.
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f }; // Defining the up direction of the camera Y axis. We only define Y so that we can assure that the camera doesn't tilt in unwanted directions.
    camera.fovy = 45.0f; // Field of view in degrees.
    camera.projection = CAMERA_PERSPECTIVE; // Setting the camera perspective to be in a 3D view.

    // Loading the font in a ttf file 

    Font doomFont = LoadFontEx("fonts/AmazDooMLeft2.ttf", 100, 0, 250);

    Image image = LoadImage("resources/Main_Game/cubicmac_Mesh.png"); // Loads the images for our 3D map.
    Texture2D cubicmap = LoadTextureFromImage(image); // Converts the image into a texture to be seen in 3D.
    Mesh mesh = GenMeshCubicmap(image, (Vector3){ 1.0f, 1.0f, 1.0f }); // Generates a 3D mesh based on the cubic map image, where each pixel represents a block.
    Model model = LoadModelFromMesh(mesh); //Creates a model from the mesh.
    Texture2D texture = LoadTexture("resources/Main_Game/cubicmap_atlas.png"); // Loads a texture for the model.
    model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture; //Basically this sets how the 3d texture will appear at the back of our buttons (ie the color the way the material of the texture is and more).
    Vector3 mapPosition = { -16.0f, 0.0f, -8.0f }; //The position of the map in real time on the screen.
    UnloadImage(image); //unloads the image when it is used.


    //Defining the 3 buttons on the title screen and their positions respective to the screen width and height.
    // Rectangle startButton = { screenWidth / 2 - 100, screenHeight / 2 - 100, 200, 50 }; 
    // Rectangle creditsButton = { screenWidth / 2 - 100, screenHeight / 2, 200, 50 };
    // Rectangle exitButton = { screenWidth / 2 - 100, screenHeight / 2 + 100, 200, 50 };

    // Button height and sizes.
    int buttonHeight = 60;
    int titleButtonHeight = 150;

    // Font sizes
    int titleFontSize = 120;
    int buttonFontSize = 40;
    int textSpacing = 2;

    // Setting the font style and size
    Vector2 titleTextSize = MeasureTextEx(doomFont, "DOOM REMAKE", titleFontSize, textSpacing);
    Vector2 startTextSize = MeasureTextEx(doomFont, "Start Game", buttonFontSize, textSpacing);
    Vector2 creditsTextSize = MeasureTextEx(doomFont, "Credits", buttonFontSize, textSpacing);
    Vector2 exitTextSize = MeasureTextEx(doomFont, "Exit Game", buttonFontSize, textSpacing);

    // Calculating button positions and sizes
    Rectangle titleLogo = {
        screenWidth / 2 - titleTextSize.x / 2 - 20,
        screenHeight / 2 - 300,
        titleTextSize.x + 40,
        titleButtonHeight
    };

    Rectangle startButton = {
        screenWidth / 2 - startTextSize.x / 2 - 20,
        screenHeight / 2 - 100,
        startTextSize.x + 40,
        buttonHeight
    };

    Rectangle creditsButton = {
        screenWidth / 2 - creditsTextSize.x / 2 - 20,
        screenHeight / 2 + 20,
        creditsTextSize.x + 40,
        buttonHeight
    };

    Rectangle exitButton = {
        screenWidth / 2 - exitTextSize.x / 2 - 20,
        screenHeight / 2 + 140,
        exitTextSize.x + 40,
        buttonHeight
    };

    SetTargetFPS(60); //Setting the fps to 60.
    Sound titleTheme = LoadSound("resources/audio/TitleMusic.mp3"); // Starting the title music.
    PlaySound(titleTheme);

    while (!WindowShouldClose()) {
        Vector2 mousePoint = GetMousePosition(); // Getting the location of the mouse pointer in real time

        // Button click handling
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) { // Using mouse button left click as cofirmation.
            if (CheckCollisionPointRec(mousePoint, startButton)) { // If start button is clicked then:
                StopSound(titleTheme);
                start_Game(); // **CHANGE GAME STATE: start_game()**
            } else if (CheckCollisionPointRec(mousePoint, creditsButton)) { // If credits button is clicked then:
                StopSound(titleTheme);
                credits(); // **CHANGE GAME STATE: credits()**
            } else if (CheckCollisionPointRec(mousePoint, exitButton)) { // If the exit button is clicked then:
                StopSound(titleTheme);
                break; // EXIT THE GAME
            }
        }

        UpdateCamera(&camera, CAMERA_ORBITAL); // Updating the camera such that it orbits around the mesh that was created of the map.

        // Rendering
        BeginDrawing();
            ClearBackground(DARKBROWN);
            BeginMode3D(camera);
                DrawModel(model, mapPosition, 1.0f, WHITE);
            EndMode3D();

            // Draw buttons and text
            DrawRectangleRec(startButton, LIGHTGRAY);
            DrawRectangleRec(creditsButton, LIGHTGRAY);
            DrawRectangleRec(exitButton, LIGHTGRAY);
            DrawRectangleRec(titleLogo, BLANK);

            // Draw texts with updated sizes
            DrawTextEx(doomFont, "DOOM REMAKE", (Vector2){ titleLogo.x + 20, titleLogo.y + 20 }, titleFontSize, textSpacing, BLACK);
            DrawTextEx(doomFont, "Start Game", (Vector2){ startButton.x + 20, startButton.y + 10 }, buttonFontSize, textSpacing, BLACK);
            DrawTextEx(doomFont, "Credits", (Vector2){ creditsButton.x + 20, creditsButton.y + 10 }, buttonFontSize, textSpacing, BLACK);
            DrawTextEx(doomFont, "Exit Game", (Vector2){ exitButton.x + 20, exitButton.y + 10 }, buttonFontSize, textSpacing, BLACK);

            DrawFPS(10, 10);
        EndDrawing();
    }


    // Unloading everything once it has been used.
    UnloadSound(titleTheme);
    UnloadTexture(cubicmap);
    UnloadTexture(texture);
    UnloadModel(model);
    return 0;
}

//------------------------------------------------------------------------------------
// Game main entry point
//------------------------------------------------------------------------------------
int start_Game(void) {
    const int screenWidth = 1600;
    const int screenHeight = 900;
    Bullet bullets[MAX_BULLETS] = { 0 }; // Setting an array of type bullet of size MAX_BULLETS of {0}.
    int bulletCount = 0; // Initializing bullet count.

    Camera camera = { 0 };
    camera.position = (Vector3){ 0.6f, 0.6f, 0.6f };
    camera.target = (Vector3){ 0.185f, 0.4f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    Image imMap = LoadImage("resources/Main_Game/cubicmac_Mesh.png");
    Texture2D cubicmap = LoadTextureFromImage(imMap);
    Mesh mesh = GenMeshCubicmap(imMap, (Vector3){ 1.0f, 1.0f, 1.0f });
    Model model = LoadModelFromMesh(mesh);
    Texture2D texture = LoadTexture("resources/Main_Game/cubicmap_atlas.png");
    Texture2D Enemy_one = LoadTexture("resources/Main_Game/Enemy.png");
    model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
    Color *mapPixels = LoadImageColors(imMap);
    UnloadImage(imMap);

    Font doomFont = LoadFontEx("fonts/AmazDooMLeft2.ttf", 100, 0, 250);

    Vector3 mapPosition = { -16.0f, 0.0f, -8.0f };
    DisableCursor(); // Here we disable the cursor for a more in depth and nice experience for the gamer and to move the camera.
    SetTargetFPS(60);
    Vector3 enemy_one_position = { 5.0f, 0.5f, 1.0f }; // Offset from camera. This is defining the position of enemy one with respect to the camera.
    // Vector3 enemy_two_position = { 5.0f, 0.5f, 1.0f }; // Offset from camera
    float size = 1.0f; //Size of enemy.

    Mesh cubeMesh = GenMeshCube(size, size, size); // Generate cube mesh
    Model EnemyModel = LoadModelFromMesh(cubeMesh);
    SetMaterialTexture(&EnemyModel.materials[0], MATERIAL_MAP_ALBEDO, Enemy_one);
    rlSetBlendFactorsSeparate(RL_SRC_ALPHA, RL_ONE_MINUS_SRC_ALPHA, RL_ONE, RL_ONE, RL_FUNC_ADD, RL_MAX);

    // Sound reloadSound = LoadSound();
    Sound gameTheme = LoadSound("resources/audio/DoomMusic.mp3");
    PlaySound(gameTheme);

    // Loading the shooting sound
    Sound shootingSound = LoadSound("resources/audio/Gunshot Sound Effect Single Shot.mp3");

    // Reload Window Setup
    int ReloadButtonHeight = 150;
    int ReloadButtonFontSize = 120;

    Vector2 ReloadTextSize = MeasureTextEx(doomFont, "RELOAD NOW PUNK!! PRESS [R] TO RELOAD!", ReloadButtonFontSize, 2);

    Rectangle ReloadSign = {
        screenWidth / 2 - ReloadTextSize.x / 2 - 20,
        screenHeight / 2 - 300,
        ReloadTextSize.x + 40,
        ReloadButtonHeight
    };    


    //This is the code for the health bar.
    Vector2 life = MeasureTextEx(doomFont,"100",50,2);

    Rectangle LifeBar = {
        50,                     //x
        50,                     //y
        life.x + 50,            //width
        50                      //height
    };

    static char scoreText[5];

    Vector2 scoreIndicator = MeasureTextEx(doomFont,"100",50,2);

    Rectangle ScoreBar = {
        GetScreenWidth() - (scoreIndicator.x + 50), 
        10,                                   
        scoreIndicator.x + 40,                     
        scoreIndicator.y + 20                      
    };

    // MAIN GAME CODE
    while (!WindowShouldClose()) {

        static int health = 100;
        static char healthText[5];


        float deltaTime = GetFrameTime(); // Get time since the last frame
        elapsedTime += deltaTime; 


        Vector3 oldCamPos = camera.position;
        UpdateCamera(&camera, CAMERA_FIRST_PERSON);


        //The following code is to add the enemy to the game map.

        Vector3 directionToCamera = { //This is to set the enemy position to the camera.
            camera.position.x - enemy_one_position.x,
            camera.position.y - enemy_one_position.y,
            camera.position.z - enemy_one_position.z
        };
         float angleY = atan2(directionToCamera.x, directionToCamera.z) * (180.0f / PI); // Convert radians to degrees


        //The following code is to make sure that the first person does not phase through walls in the game.
        // Now check for collision after updating the camera position
        Vector2 playerPos = { camera.position.x, camera.position.z }; // Player's position in the XY plane
        float playerRadius = 0.1f; // Radius of the player's collision detection area

        int collided = 0;  // Flag to track if a collision occurred

        // Check collision for each map pixel
        for (int y = 0; y < cubicmap.height; y++) {
            for (int x = 0; x < cubicmap.width; x++) {
                if ((mapPixels[y * cubicmap.width + x].r == 255) && // Check if it's a wall (red pixel)
                    CheckCollisionCircleRec(playerPos, playerRadius, (Rectangle) {
                        mapPosition.x - 0.5f + x * 1.0f, mapPosition.z - 0.5f + y * 1.0f, 1.0f, 1.0f })) {
                    
                    // If collision is detected, set the collision flag and adjust camera position
                    collided = 1;
                    break;  // No need to check further, break out of inner loop
                }
            }
            if (collided) break; // Break out of outer loop if collision was found
        }

        // If collision occurred, adjust the camera to prevent passing through the wall
        if (collided) {
            // Reset the camera's position to the old position but only along the axis that collided
            if (camera.position.x != oldCamPos.x) {
                camera.position.x = oldCamPos.x; // Prevent movement in X direction if collided
            }
            if (camera.position.z != oldCamPos.z) {
                camera.position.z = oldCamPos.z; // Prevent movement in Z direction if collided
            }
        }        
        // PHASING THROUGH WALLS CHECK ENDS HERE

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) { // If the left mouse button is pressed the call the shoot button and then shoot the bullet.
            Shoot(bullets, &bulletCount, camera);
            PlaySound(shootingSound);
        }

        float delta = GetFrameTime(); // Getting the current time and then storing it in delta to use later on in the UpdateBullets function.
        UpdateBullets(bullets, bulletCount, delta);

        BeginDrawing();
            ClearBackground(DARKGRAY);
            BeginMode3D(camera);
                DrawModel(model, mapPosition, 1.0f, WHITE);
                DrawBullets(bullets, bulletCount); // **Change: Added bullet drawing in game loop**

            //The following code handles how the enemy will move towards the camera and follow the first person around.
            if (camera.position.x - enemy_one_position.x > 0.1) {
                enemy_one_position.x += 0.025;
            }
            if (camera.position.z - enemy_one_position.z > 0.1) {
                enemy_one_position.z += 0.025;
            }
        
            if (camera.position.x - enemy_one_position.x < -0.1) {
                enemy_one_position.x -= 0.025;
            }
            if (camera.position.z - enemy_one_position.z < -0.1) {
                enemy_one_position.z -= 0.025;
            }
            //The following code renders the enemy model. 
            BeginBlendMode(RL_BLEND_CUSTOM_SEPARATE); //A custom blend for the enemy model.
            DrawModelEx(EnemyModel, enemy_one_position, (Vector3){0.0f, 1.0f, 0.0f}, angleY, (Vector3){1.0f, 1.0f, 1.0f}, WHITE); //Renders the enemy model.
                EndBlendMode();
               DrawModel(model, mapPosition, 1.0f, WHITE); //Enemy_two
            //    DrawCubeTexture(Enemy_one,enemy_one_position, 0.0f, 1.0f, 1.0f, WHITE);
            //    DrawCubeTexture(Enemy_two,enemy_two_position, 0.0f, 1.0f, 1.0f, WHITE);
            EndMode3D();





            //The following code is for adding a reload feature to the game.
            if (bulletCount >= MAX_BULLETS) {
                // Display reload message on the screen
                DrawRectangleRec(ReloadSign, BLANK);
                DrawTextEx(doomFont, "RELOAD NOW PUNK!! PRESS [R] TO RELOAD!", (Vector2){ ReloadSign.x + 20, ReloadSign.y + 20 }, ReloadButtonFontSize, 2, BLACK);

                // Check for both lowercase 'r' and uppercase 'R'
                if (IsKeyPressed(KEY_R)) {
                    bulletCount = 0;
                }
            }

            if (!gameOver) {
                for (int i = 0; i < bulletCount; i++) {
                    if (bullets[i].active) {
                        float BULLET_SPEED = 10.0f;
                        bullets[i].position.x += bullets[i].direction.x * BULLET_SPEED * GetFrameTime();
                        bullets[i].position.y += bullets[i].direction.y * BULLET_SPEED * GetFrameTime();
                        bullets[i].position.z += bullets[i].direction.z * BULLET_SPEED * GetFrameTime();

                        // Deactivate bullet if it goes out of bounds
                        if (bullets[i].position.x > 100 || bullets[i].position.x < -100 ||
                            bullets[i].position.z > 100 || bullets[i].position.z < -100) {
                            bullets[i].active = false;
                        }
                    }
                }

                if (elapsedTime >= 10.0f) {
                    score += 10;                  // Increase score by 10
                    elapsedTime -= 10.0f;         // Reset the timer (preserve leftover time)
                    sprintf(scoreText,"%d",score);         
                }
                DrawRectangleRec(ScoreBar,RED);
                DrawTextEx(doomFont, scoreText, 
                    (Vector2){ScoreBar.x + 20, ScoreBar.y + 10}, 
                    50,  
                    2,   
                    BLACK
                );
                DrawRectangleRec(LifeBar, RED);
                int funcCount = CheckCollisionAndShooting(bullets, bulletCount, camera, &enemy_one_position);

                switch(funcCount){
                    case 100:
                        health = health - 10;
                        break;
                    case 200:
                        health = health - 10;
                        break;
                    case 300:
                        health = health - 10;
                        break;
                    case 400:
                        health = health - 10;
                        break;
                    case 500:
                        health = health -10;
                        break;
                    case 600:
                        health = health -10;
                        break;
                    case 700:
                        health = health -10;
                        break;
                    case 800:
                        health = health -10;
                        break;
                    case 900:
                        health = health -10;
                        break;
                    case 1000:
                        health = health -10;
                        break;
                }

                sprintf(healthText,"%d", health);
                DrawTextEx(doomFont, healthText, 
                    (Vector2){LifeBar.x + 20, LifeBar.y + 10}, 
                    50,  
                    2,   
                    BLACK
                );

                if(funcCount == 1000){
                    gameOver = true;
                }
                

                // Handle enemy respawn
                if (enemyHit) {
                    respawnTimer -= GetFrameTime();
                    if (respawnTimer <= 0.0f) {
                        // Reset enemy hit status and position
                        enemyHit = false;
                        enemy_one_position = (Vector3){ 5.0f, 0.5f, 1.0f }; // Reset enemy position

                        // Optionally, reset bullet positions if needed
                        for (int i = 0; i < bulletCount; i++) {
                            if (!bullets[i].active) {
                                bullets[i].position = (Vector3){0.0f, 0.0f, 0.0f}; // Reset inactive bullet positions
                            }
                        }
                    }
                }

            }

            if (!enemyHit) {
                DrawModelEx(EnemyModel, enemy_one_position, (Vector3){0.0f, 1.0f, 0.0f}, angleY, (Vector3){1.0f, 1.0f, 1.0f}, WHITE);
            }

            if (gameOver) {
                DrawText("GAME OVER!", screenWidth / 2 - 100, screenHeight / 2, 50, RED);
                EndDrawing();
                // credits();
                return 0; // Stop further rendering
            }

            DrawFPS(10, 10);
        EndDrawing();
    }

    UnloadSound(shootingSound);
    UnloadSound(gameTheme);
    UnloadTexture(cubicmap);
    UnloadTexture(texture);
    UnloadModel(model);
    UnloadModel(EnemyModel);
    return 0;
}

//------------------------------------------------------------------------------------
// Shooting mechanics
//------------------------------------------------------------------------------------

//The following function is the shoot function. It runs when the user shoots a bullet and define's it's position and direction.
void Shoot(Bullet bullets[], int *bulletCount, Camera camera) {
    if (*bulletCount < MAX_BULLETS) { // Seeing if the current bullet count is less than the magazine size.
        bullets[*bulletCount].position = camera.position; // Updating the bullet's current position to where the user is standing.
        bullets[*bulletCount].direction = Vector3Normalize(Vector3Subtract(camera.target, camera.position)); // **Change: Updated bullet direction to use camera's forward vector**
        bullets[*bulletCount].active = true; // Making it so that the program knows that a bullet is now active.
        (*bulletCount)++; //incrementing bullet count.
    }
}

// The following function updates the position of each active bullet and deactivates bullets that have traveled beyond a certain distance.
void UpdateBullets(Bullet bullets[], int bulletCount, float delta) {
    for (int i = 0; i < bulletCount; i++) { // Loops through the bullets using bullet count as an upper bound.
        if (bullets[i].active) { // If a bullet is indeed fired then proceed with the following code:
            bullets[i].position = Vector3Add(bullets[i].position, Vector3Scale(bullets[i].direction, 20.0f * delta)); //  Scales the direction vector by the speed (20.0f) and the time delta (delta), ensuring consistent movement regardless of frame rate. Vector3add() adds this scaled direction vector to the current position, moving the bullet forward.
            if (Vector3Length(bullets[i].position) > 50.0f) { // If the bullet goes beyond 50 units then the following bullet is deactivated.
                bullets[i].active = false; // Set the current bullet to be active = false.
            }
        }
    }
}

// Renders te bullets on the screen.
void DrawBullets(Bullet bullets[], int bulletCount) {
    for (int i = 0; i < bulletCount; i++) { // Loops through the bullets using bullet count as an upper bound.
        if (bullets[i].active) { // If a bullet is indeed active then:
            DrawSphere(bullets[i].position, 0.1f, RED); //Draw a sphere that is the shape of a bullet.
        }
    }
}

// The following function is a check to see if a bullet hits an enemy.

int CheckCollisionAndShooting(Bullet bullets[], int bulletCount, Camera camera, Vector3 *enemyPosition) {
    static int lifeCount = 0;

    // Check for collision with the player (first-person character)
    float playerRadius = 0.3f; // Adjust radius as needed
    if (CheckCollisionCircles((Vector2){camera.position.x, camera.position.z}, playerRadius,
                              (Vector2){enemyPosition->x, enemyPosition->z}, enemyRadius)) {
        lifeCount ++;
        return lifeCount;
        // if(lifeCount == 1000){
        //             gameOver = true;
        //             return;
        // }
        // return; // End the game if collision occurs
    }

    //Check if any bullet hits the enemy
    for (int i = 0; i < bulletCount; i++) {
        if (bullets[i].active && CheckCollisionBulletEnemy(bullets[i].position, *enemyPosition, enemyRadius)) {
            enemyHit = true;
            respawnTimer = 5.0f; // Start the respawn timer
            bullets[i].active = false;
            bullets[i].position = (Vector3){0.0f, 0.0f, 0.0f}; //resetting the bullet position
            break;
        }
    }


}


//Function to check collision between a point (bullet position) and a sphere (enemy position)
bool CheckCollisionBulletEnemy(Vector3 bulletPos, Vector3 enemyPos, float enemyRadius) {
    float distX = bulletPos.x - enemyPos.x;
    float distZ = bulletPos.z - enemyPos.z;
    float distance = sqrt(distX * distX + distZ * distZ); // 2D distance between bullet and enemy (ignoring Y)

    // If the distance is less than the sum of the radii, a collision occurred
    float bulletRadius = 0.1f; // Adjust this value as needed
    return distance < (enemyRadius + bulletRadius);
}



//------------------------------------------------------------------------------------
// Enemy Generation
//------------------------------------------------------------------------------------


// The following function describes the render of the the enemy from all sides.
void DrawCubeTexture(Texture2D texture, Vector3 position, float width, float height, float length, Color color)
{
    float x = position.x;
    float y = position.y;
    float z = position.z;

    // Set desired texture to be enabled while drawing following vertex data
    rlSetTexture(texture.id);

    // Vertex data transformation can be defined with the commented lines,
    // but in this example we calculate the transformed vertex data directly when calling rlVertex3f()
    //rlPushMatrix();
        // NOTE: Transformation is applied in inverse order (scale -> rotate -> translate)
        //rlTranslatef(2.0f, 0.0f, 0.0f);
        //rlRotatef(45, 0, 1, 0);
        //rlScalef(2.0f, 2.0f, 2.0f);

        rlBegin(RL_QUADS);
            rlColor4ub(color.r, color.g, color.b, color.a);
            // Front Face
            rlNormal3f(0.0f, 0.0f, 1.0f);       // Normal Pointing Towards Viewer
            rlTexCoord2f(0.0f, 0.0f); rlVertex3f(x - width/2, y - height/2, z + length/2);  // Bottom Left Of The Texture and Quad
            rlTexCoord2f(1.0f, 0.0f); rlVertex3f(x + width/2, y - height/2, z + length/2);  // Bottom Right Of The Texture and Quad
            rlTexCoord2f(1.0f, 1.0f); rlVertex3f(x + width/2, y + height/2, z + length/2);  // Top Right Of The Texture and Quad
            rlTexCoord2f(0.0f, 1.0f); rlVertex3f(x - width/2, y + height/2, z + length/2);  // Top Left Of The Texture and Quad
            // Back Face
            rlNormal3f(0.0f, 0.0f, - 1.0f);     // Normal Pointing Away From Viewer
            rlTexCoord2f(1.0f, 0.0f); rlVertex3f(x - width/2, y - height/2, z - length/2);  // Bottom Right Of The Texture and Quad
            rlTexCoord2f(1.0f, 1.0f); rlVertex3f(x - width/2, y + height/2, z - length/2);  // Top Right Of The Texture and Quad
            rlTexCoord2f(0.0f, 1.0f); rlVertex3f(x + width/2, y + height/2, z - length/2);  // Top Left Of The Texture and Quad
            rlTexCoord2f(0.0f, 0.0f); rlVertex3f(x + width/2, y - height/2, z - length/2);  // Bottom Left Of The Texture and Quad
            // Top Face
            rlNormal3f(0.0f, 1.0f, 0.0f);       // Normal Pointing Up
            rlTexCoord2f(0.0f, 1.0f); rlVertex3f(x - width/2, y + height/2, z - length/2);  // Top Left Of The Texture and Quad
            rlTexCoord2f(0.0f, 0.0f); rlVertex3f(x - width/2, y + height/2, z + length/2);  // Bottom Left Of The Texture and Quad
            rlTexCoord2f(1.0f, 0.0f); rlVertex3f(x + width/2, y + height/2, z + length/2);  // Bottom Right Of The Texture and Quad
            rlTexCoord2f(1.0f, 1.0f); rlVertex3f(x + width/2, y + height/2, z - length/2);  // Top Right Of The Texture and Quad
            // Bottom Face
            rlNormal3f(0.0f, - 1.0f, 0.0f);     // Normal Pointing Down
            rlTexCoord2f(1.0f, 1.0f); rlVertex3f(x - width/2, y - height/2, z - length/2);  // Top Right Of The Texture and Quad
            rlTexCoord2f(0.0f, 1.0f); rlVertex3f(x + width/2, y - height/2, z - length/2);  // Top Left Of The Texture and Quad
            rlTexCoord2f(0.0f, 0.0f); rlVertex3f(x + width/2, y - height/2, z + length/2);  // Bottom Left Of The Texture and Quad
            rlTexCoord2f(1.0f, 0.0f); rlVertex3f(x - width/2, y - height/2, z + length/2);  // Bottom Right Of The Texture and Quad
            // Right face
            rlNormal3f(1.0f, 0.0f, 0.0f);       // Normal Pointing Right
            rlTexCoord2f(1.0f, 0.0f); rlVertex3f(x + width/2, y - height/2, z - length/2);  // Bottom Right Of The Texture and Quad
            rlTexCoord2f(1.0f, 1.0f); rlVertex3f(x + width/2, y + height/2, z - length/2);  // Top Right Of The Texture and Quad
            rlTexCoord2f(0.0f, 1.0f); rlVertex3f(x + width/2, y + height/2, z + length/2);  // Top Left Of The Texture and Quad
            rlTexCoord2f(0.0f, 0.0f); rlVertex3f(x + width/2, y - height/2, z + length/2);  // Bottom Left Of The Texture and Quad
            // Left Face
            rlNormal3f( - 1.0f, 0.0f, 0.0f);    // Normal Pointing Left
            rlTexCoord2f(0.0f, 0.0f); rlVertex3f(x - width/2, y - height/2, z - length/2);  // Bottom Left Of The Texture and Quad
            rlTexCoord2f(1.0f, 0.0f); rlVertex3f(x - width/2, y - height/2, z + length/2);  // Bottom Right Of The Texture and Quad
            rlTexCoord2f(1.0f, 1.0f); rlVertex3f(x - width/2, y + height/2, z + length/2);  // Top Right Of The Texture and Quad
            rlTexCoord2f(0.0f, 1.0f); rlVertex3f(x - width/2, y + height/2, z - length/2);  // Top Left Of The Texture and Quad
        rlEnd();
    //rlPopMatrix();

    rlSetTexture(0);
}

// Draw cube with texture piece applied to all faces
void DrawCubeTextureRec(Texture2D texture, Rectangle source, Vector3 position, float width, float height, float length, Color color)
{
    float x = position.x;
    float y = position.y;
    float z = position.z;
    float texWidth = (float)texture.width;
    float texHeight = (float)texture.height;

    // Set desired texture to be enabled while drawing following vertex data
    rlSetTexture(texture.id);

    // We calculate the normalized texture coordinates for the desired texture-source-rectangle
    // It means converting from (tex.width, tex.height) coordinates to [0.0f, 1.0f] equivalent 
    rlBegin(RL_QUADS);
        rlColor4ub(color.r, color.g, color.b, color.a);

        // Front face
        rlNormal3f(0.0f, 0.0f, 1.0f);
        rlTexCoord2f(source.x/texWidth, (source.y + source.height)/texHeight);
        rlVertex3f(x - width/2, y - height/2, z + length/2);
        rlTexCoord2f((source.x + source.width)/texWidth, (source.y + source.height)/texHeight);
        rlVertex3f(x + width/2, y - height/2, z + length/2);
        rlTexCoord2f((source.x + source.width)/texWidth, source.y/texHeight);
        rlVertex3f(x + width/2, y + height/2, z + length/2);
        rlTexCoord2f(source.x/texWidth, source.y/texHeight);
        rlVertex3f(x - width/2, y + height/2, z + length/2);

        // Back face
        rlNormal3f(0.0f, 0.0f, - 1.0f);
        rlTexCoord2f((source.x + source.width)/texWidth, (source.y + source.height)/texHeight);
        rlVertex3f(x - width/2, y - height/2, z - length/2);
        rlTexCoord2f((source.x + source.width)/texWidth, source.y/texHeight);
        rlVertex3f(x - width/2, y + height/2, z - length/2);
        rlTexCoord2f(source.x/texWidth, source.y/texHeight);
        rlVertex3f(x + width/2, y + height/2, z - length/2);
        rlTexCoord2f(source.x/texWidth, (source.y + source.height)/texHeight);
        rlVertex3f(x + width/2, y - height/2, z - length/2);

        // Top face
        rlNormal3f(0.0f, 1.0f, 0.0f);
        rlTexCoord2f(source.x/texWidth, source.y/texHeight);
        rlVertex3f(x - width/2, y + height/2, z - length/2);
        rlTexCoord2f(source.x/texWidth, (source.y + source.height)/texHeight);
        rlVertex3f(x - width/2, y + height/2, z + length/2);
        rlTexCoord2f((source.x + source.width)/texWidth, (source.y + source.height)/texHeight);
        rlVertex3f(x + width/2, y + height/2, z + length/2);
        rlTexCoord2f((source.x + source.width)/texWidth, source.y/texHeight);
        rlVertex3f(x + width/2, y + height/2, z - length/2);

        // Bottom face
        rlNormal3f(0.0f, - 1.0f, 0.0f);
        rlTexCoord2f((source.x + source.width)/texWidth, source.y/texHeight);
        rlVertex3f(x - width/2, y - height/2, z - length/2);
        rlTexCoord2f(source.x/texWidth, source.y/texHeight);
        rlVertex3f(x + width/2, y - height/2, z - length/2);
        rlTexCoord2f(source.x/texWidth, (source.y + source.height)/texHeight);
        rlVertex3f(x + width/2, y - height/2, z + length/2);
        rlTexCoord2f((source.x + source.width)/texWidth, (source.y + source.height)/texHeight);
        rlVertex3f(x - width/2, y - height/2, z + length/2);

        // Right face
        rlNormal3f(1.0f, 0.0f, 0.0f);
        rlTexCoord2f((source.x + source.width)/texWidth, (source.y + source.height)/texHeight);
        rlVertex3f(x + width/2, y - height/2, z - length/2);
        rlTexCoord2f((source.x + source.width)/texWidth, source.y/texHeight);
        rlVertex3f(x + width/2, y + height/2, z - length/2);
        rlTexCoord2f(source.x/texWidth, source.y/texHeight);
        rlVertex3f(x + width/2, y + height/2, z + length/2);
        rlTexCoord2f(source.x/texWidth, (source.y + source.height)/texHeight);
        rlVertex3f(x + width/2, y - height/2, z + length/2);

        // Left face
        rlNormal3f( - 1.0f, 0.0f, 0.0f);
        rlTexCoord2f(source.x/texWidth, (source.y + source.height)/texHeight);
        rlVertex3f(x - width/2, y - height/2, z - length/2);
        rlTexCoord2f((source.x + source.width)/texWidth, (source.y + source.height)/texHeight);
        rlVertex3f(x - width/2, y - height/2, z + length/2);
        rlTexCoord2f((source.x + source.width)/texWidth, source.y/texHeight);
        rlVertex3f(x - width/2, y + height/2, z + length/2);
        rlTexCoord2f(source.x/texWidth, source.y/texHeight);
        rlVertex3f(x - width/2, y + height/2, z - length/2);

    rlEnd();

    rlSetTexture(0);
}

int credits(void) {
    InitWindow(1600, 900, "Credits");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawText("Credits", 750, 450, 40, BLACK);
            DrawText("Press ESC to return", 650, 500, 20, GRAY);
        EndDrawing();

        if (IsKeyPressed(KEY_ESCAPE)) break;
    }
    return 0;
}
