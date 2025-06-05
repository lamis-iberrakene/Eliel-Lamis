#include "raylib.h"
#include <stdlib.h>
#include <stdbool.h>

#define CELL_SIZE 20
#define MAX_SNAKE_LENGTH 100
#define MAX_WALLS 10

typedef struct {
    int x, y;
} Vector2Int;

typedef struct {
    Vector2Int position;
    Color color;
    int scoreValue;
} Fruit;

typedef struct {
    Vector2Int body[MAX_SNAKE_LENGTH];
    int length;
    Vector2Int direction;
    Color headColor;
    Color bodyColor;
    int score;
    int speedBoostTimer;
} Snake;

typedef enum { MENU, SOLO, DUO, PAUSE, GAME_OVER } GameState;

bool Vector2IntEquals(Vector2Int a, Vector2Int b) {
    return a.x == b.x && a.y == b.y;
}

bool IsOnSnake(Vector2Int pos, Snake snake) {
    for (int i = 0; i < snake.length; i++) {
        if (Vector2IntEquals(pos, snake.body[i])) return true;
    }
    return false;
}

bool IsOnWalls(Vector2Int pos, Vector2Int *walls, int wallCount) {
    for (int i = 0; i < wallCount; i++) {
        if (Vector2IntEquals(pos, walls[i])) return true;
    }
    return false;
}

void GenerateFruit(Fruit *fruit, int cols, int rows, Snake s1, Snake s2, Vector2Int *walls, int wallCount) {
    do {
        fruit->position.x = GetRandomValue(0, cols - 1);
        fruit->position.y = GetRandomValue(0, rows - 1);
    } while (IsOnSnake(fruit->position, s1) || IsOnSnake(fruit->position, s2) || IsOnWalls(fruit->position, walls, wallCount));

    int type = GetRandomValue(0, 9);
    if (type <= 6) {
        fruit->color = RED;
        fruit->scoreValue = 10;
    } else if (type <= 8) {
        fruit->color = BLUE;
        fruit->scoreValue = 20;
    } else {
        fruit->color = BLACK;
        fruit->scoreValue = -10;
    }
}

void GenerateWalls(Vector2Int *walls, int *wallCount, int max, Snake s1, Snake s2, Fruit fruit, int cols, int rows) {
    if (*wallCount >= max) return;
    Vector2Int newWall;
    do {
        newWall.x = GetRandomValue(0, cols - 1);
        newWall.y = GetRandomValue(0, rows - 1);
    } while (IsOnSnake(newWall, s1) || IsOnSnake(newWall, s2) || Vector2IntEquals(newWall, fruit.position));

    walls[*wallCount] = newWall;
    (*wallCount)++;
}

void DrawRainbowFruit(Vector2Int position) {
    Color rainbow[7] = { RED, ORANGE, YELLOW, GREEN, BLUE, DARKBLUE, PURPLE };
    int stripeWidth = CELL_SIZE / 7;
    for (int i = 0; i < 7; i++) {
        DrawRectangle(position.x * CELL_SIZE + i * stripeWidth, position.y * CELL_SIZE, stripeWidth, CELL_SIZE, rainbow[i]);
    }
}

int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 600;
    const int cols = screenWidth / CELL_SIZE;
    const int rows = screenHeight / CELL_SIZE;

    InitWindow(screenWidth, screenHeight, "Snake Game - Solo & Duo");
    SetTargetFPS(10);

    GameState gameState = MENU;
    Snake player1 = {0}, player2 = {0};
    Fruit fruit = {0};
    Vector2Int walls[MAX_WALLS];
    int wallCount = 0;

    bool paused = false;
    bool winner = false;
    int mode = 0;

    int chehTimer = 0; // Durée d'affichage du message "cheh -10"

    while (!WindowShouldClose()) {
        if (gameState == MENU) {
            if (IsKeyPressed(KEY_ONE)) {
                mode = 1;
                gameState = SOLO;
                player1 = (Snake){{0}, 5, {1, 0}, DARKGREEN, GREEN, 0, 0};
                for (int i = 0; i < 5; i++) {
                    player1.body[i] = (Vector2Int){cols / 2 - i, rows / 2};
                }
                wallCount = 0;
                GenerateFruit(&fruit, cols, rows, player1, player2, walls, wallCount);
                chehTimer = 0;
            } else if (IsKeyPressed(KEY_TWO)) {
                mode = 2;
                gameState = DUO;
                player1 = (Snake){{0}, 5, {1, 0}, DARKGREEN, GREEN, 0, 0};
                player2 = (Snake){{0}, 5, {-1, 0}, DARKBLUE, BLUE, 0, 0};
                for (int i = 0; i < 5; i++) {
                    player1.body[i] = (Vector2Int){cols / 4 - i, rows / 2};
                    player2.body[i] = (Vector2Int){3 * cols / 4 + i, rows / 2};
                }
                wallCount = 0;
                GenerateFruit(&fruit, cols, rows, player1, player2, walls, wallCount);
                chehTimer = 0;
            }

            BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawText("SNAKE GAME", 300, 100, 40, DARKGREEN);
            DrawText("1. Solo", 350, 200, 30, BLACK);
            DrawText("2. Duo", 350, 250, 30, BLACK);
            EndDrawing();
            continue;
        }

        if (gameState == SOLO || gameState == DUO) {
            if (IsKeyPressed(KEY_ESCAPE)) {
                paused = !paused;
                gameState = paused ? PAUSE : (mode == 1 ? SOLO : DUO);
            }

            if (!paused) {
                if (IsKeyPressed(KEY_UP) && player1.direction.y == 0) player1.direction = (Vector2Int){0, -1};
                if (IsKeyPressed(KEY_DOWN) && player1.direction.y == 0) player1.direction = (Vector2Int){0, 1};
                if (IsKeyPressed(KEY_LEFT) && player1.direction.x == 0) player1.direction = (Vector2Int){-1, 0};
                if (IsKeyPressed(KEY_RIGHT) && player1.direction.x == 0) player1.direction = (Vector2Int){1, 0};

                if (mode == 2) {
                    if (IsKeyPressed(KEY_W) && player2.direction.y == 0) player2.direction = (Vector2Int){0, -1};
                    if (IsKeyPressed(KEY_S) && player2.direction.y == 0) player2.direction = (Vector2Int){0, 1};
                    if (IsKeyPressed(KEY_A) && player2.direction.x == 0) player2.direction = (Vector2Int){-1, 0};
                    if (IsKeyPressed(KEY_D) && player2.direction.x == 0) player2.direction = (Vector2Int){1, 0};
                }

                for (int i = player1.length - 1; i > 0; i--) player1.body[i] = player1.body[i - 1];
                player1.body[0].x += player1.direction.x;
                player1.body[0].y += player1.direction.y;

                if (mode == 2) {
                    for (int i = player2.length - 1; i > 0; i--) player2.body[i] = player2.body[i - 1];
                    player2.body[0].x += player2.direction.x;
                }

                for (int i = 1; i < player1.length; i++) if (Vector2IntEquals(player1.body[0], player1.body[i])) gameState = GAME_OVER;
                if (player1.body[0].x < 0 || player1.body[0].x >= cols || player1.body[0].y < 0 || player1.body[0].y >= rows) gameState = GAME_OVER;
                if (IsOnWalls(player1.body[0], walls, wallCount)) gameState = GAME_OVER;

                if (mode == 2) {
                    for (int i = 1; i < player2.length; i++) if (Vector2IntEquals(player2.body[0], player2.body[i])) gameState = GAME_OVER;
                    if (player2.body[0].x < 0 || player2.body[0].x >= cols || player2.body[0].y < 0 || player2.body[0].y >= rows) gameState = GAME_OVER;
                    if (IsOnWalls(player2.body[0], walls, wallCount)) gameState = GAME_OVER;

                    for (int i = 1; i < player1.length; i++) if (Vector2IntEquals(player2.body[0], player1.body[i])) { winner = true; gameState = GAME_OVER; }
                    for (int i = 1; i < player2.length; i++) if (Vector2IntEquals(player1.body[0], player2.body[i])) { winner = false; gameState = GAME_OVER; }
                }

                if (Vector2IntEquals(player1.body[0], fruit.position)) {
                    if (player1.length < MAX_SNAKE_LENGTH) player1.length++;
                    player1.score += fruit.scoreValue;
                    if (fruit.scoreValue == -10) chehTimer = 30;
                    if (ColorToInt(fruit.color) == ColorToInt(BLUE)) player1.speedBoostTimer = 50;
                    if (player1.score < 0) player1.score = 0;
                    GenerateFruit(&fruit, cols, rows, player1, player2, walls, wallCount);
                    if (player1.score / 20 > wallCount && wallCount < MAX_WALLS) GenerateWalls(walls, &wallCount, MAX_WALLS, player1, player2, fruit, cols, rows);
                }

                if (mode == 2 && Vector2IntEquals(player2.body[0], fruit.position)) {
                    if (player2.length < MAX_SNAKE_LENGTH) player2.length++;
                    player2.score += fruit.scoreValue;
                    if (fruit.scoreValue == -10) chehTimer = 30;
                    if (ColorToInt(fruit.color) == ColorToInt(BLUE)) player2.speedBoostTimer = 50;
                    if (player2.score < 0) player2.score = 0;
                    GenerateFruit(&fruit, cols, rows, player1, player2, walls, wallCount);
                    if (player2.score / 20 > wallCount && wallCount < MAX_WALLS) GenerateWalls(walls, &wallCount, MAX_WALLS, player1, player2, fruit, cols, rows);
                }

                if (chehTimer > 0) chehTimer--;
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        for (int x = 0; x < cols; x++) for (int y = 0; y < rows; y++)
            DrawRectangleLines(x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE, LIGHTGRAY);

        for (int i = 0; i < wallCount; i++) {
            DrawRectangle(walls[i].x * CELL_SIZE, walls[i].y * CELL_SIZE, CELL_SIZE, CELL_SIZE, GRAY);
        }

        if (fruit.scoreValue == -10) {
            DrawRainbowFruit(fruit.position);
        } else {
            DrawRectangle(fruit.position.x * CELL_SIZE, fruit.position.y * CELL_SIZE, CELL_SIZE, CELL_SIZE, fruit.color);
        }

        for (int i = 0; i < player1.length; i++) {
            DrawRectangle(player1.body[i].x * CELL_SIZE, player1.body[i].y * CELL_SIZE, CELL_SIZE, CELL_SIZE,
                          i == 0 ? player1.headColor : player1.bodyColor);
        }
        if (mode == 2) {
            for (int i = 0; i < player2.length; i++) {
                DrawRectangle(player2.body[i].x * CELL_SIZE, player2.body[i].y * CELL_SIZE, CELL_SIZE, CELL_SIZE,
                              i == 0 ? player2.headColor : player2.bodyColor);
            }
        }

        DrawText(TextFormat("Score P1: %d", player1.score), 10, 10, 20, BLACK);
        if (mode == 2) DrawText(TextFormat("Score P2: %d", player2.score), screenWidth - 160, 10, 20, BLACK);

        if (chehTimer > 0) {
            DrawText("cheh -10", screenWidth / 2 - 50, screenHeight / 2 - 100, 30, RED);
        }

        if (gameState == PAUSE) {
            DrawText("PAUSE", screenWidth / 2 - 50, screenHeight / 2 - 40, 40, RED);
            DrawText("R: Recommencer | Q: Quitter", screenWidth / 2 - 150, screenHeight / 2 + 10, 20, BLACK);
            if (IsKeyPressed(KEY_R)) gameState = MENU;
            else if (IsKeyPressed(KEY_Q)) gameState = MENU;
        }

        if (gameState == GAME_OVER) {
            DrawText("GAME OVER", screenWidth / 2 - 100, screenHeight / 2 - 40, 40, RED);
            if (mode == 2)
                DrawText(winner ? "Player 2 wins!" : "Player 1 wins!", screenWidth / 2 - 100, screenHeight / 2, 30, BLACK);
            DrawText("Appuie sur R pour recommencer", screenWidth / 2 - 150, screenHeight / 2 + 50, 20, BLACK);
            if (IsKeyPressed(KEY_R)) gameState = MENU;
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
