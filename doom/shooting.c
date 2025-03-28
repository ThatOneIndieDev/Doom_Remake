// #include <raylib.h>
// #include <stdio.h>

// #define MAX_BULLETS 100

// typedef struct Bullet {
//     Vector3 position;
//     Vector3 direction;
//     bool active;
// } Bullet; //Bullet is a type alias

// // Function prototypes
// void Shoot(Bullet bullets[], int *bulletCount, Camera camera);
// void UpdateBullets(Bullet bullets[], int bulletCount, float delta);
// void DrawBullets(Bullet bullets[], int bulletCount);

// int secondToMain(void) {
//     const int screenWidth = 1600;
//     const int screenHeight = 900;
//     InitWindow(screenWidth, screenHeight, "Gun Shooting Example");

//     InitAudioDevice();
//     Camera camera = { 0 };
//     camera.position = (Vector3){ 0.0f, 1.6f, 3.0f };
//     camera.target = (Vector3){ 0.0f, 1.6f, 0.0f };
//     camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
//     camera.fovy = 45.0f;
//     camera.projection = CAMERA_PERSPECTIVE;

//     Bullet bullets[MAX_BULLETS] = { 0 };
//     int bulletCount = 0;

//     SetTargetFPS(60);

//     while (!WindowShouldClose()) {
//         // Update camera
//         UpdateCamera(&camera, CAMERA_FIRST_PERSON);

//         // Check for shooting
//         if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
//             Shoot(bullets, &bulletCount, camera);
//         }

//         // Update bullets
//         float delta = GetFrameTime();
//         UpdateBullets(bullets, bulletCount, delta);

//         // Draw
//         BeginDrawing();
//             ClearBackground(RAYWHITE);
//             BeginMode3D(camera);

//             // Draw bullets
//             DrawBullets(bullets, bulletCount);

//             EndMode3D();
//             DrawText("Press left mouse button to shoot!", 10, 10, 20, DARKGRAY);
//         EndDrawing();
//     }

//     CloseAudioDevice();
//     CloseWindow();
//     return 0;
// }

// void Shoot(Bullet bullets[], int *bulletCount, Camera camera) {
//     if (*bulletCount < MAX_BULLETS) {
//         bullets[*bulletCount].position = camera.position; // Start position at the camera
//         bullets[*bulletCount].direction = (Vector3){ 0.0f, 0.0f, -1.0f }; // Shoot forward
//         bullets[*bulletCount].active = true;
//         (*bulletCount)++;
//     }
// }

// void UpdateBullets(Bullet bullets[], int bulletCount, float delta) {
//     for (int i = 0; i < bulletCount; i++) {
//         if (bullets[i].active) {
//             bullets[i].position.x += bullets[i].direction.x * 20.0f * delta; // Move bullet forward
//             bullets[i].position.y += bullets[i].direction.y * 20.0f * delta;
//             bullets[i].position.z += bullets[i].direction.z * 20.0f * delta;
//             // Add logic to deactivate bullets if they go off-screen or hit something // Hashir please do it so that when the bullets hit the object the object dissapears 
//         }
//     }
// }

// void DrawBullets(Bullet bullets[], int bulletCount) {
//     for (int i = 0; i < bulletCount; i++) {
//         if (bullets[i].active) {
//             DrawSphere(bullets[i].position, 0.1f, RED); // Draw bullet as a sphere
//         }
//     }
// }
