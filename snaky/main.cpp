#include "raylib.h"
#include <deque>
#include <vector>

// ---------- Настройки ----------
const int CELL_SIZE = 25;   // размер клетки в пикселях
const int GRID_W = 30;   // ширина поля в клетках
const int GRID_H = 24;   // высота поля в клетках
const int SCREEN_W = GRID_W * CELL_SIZE;
const int SCREEN_H = GRID_H * CELL_SIZE;
const float MOVE_DELAY = 0.12f; // секунд между шагами

// ---------- Направления ----------
enum class Dir { Up, Down, Left, Right };

int main()
{
    InitWindow(SCREEN_W, SCREEN_H, "Snake - raylib");
    SetTargetFPS(60);

    // Змейка: deque, где front — голова
    std::deque<Vector2> snake;
    snake.push_back({ GRID_W / 2,     GRID_H / 2 });
    snake.push_back({ GRID_W / 2 - 1, GRID_H / 2 });
    snake.push_back({ GRID_W / 2 - 2, GRID_H / 2 });

    Dir dir = Dir::Right;
    Dir nextDir = Dir::Right;

    Vector2 food = { GetRandomValue(0, GRID_W - 1), GetRandomValue(0, GRID_H - 1) };

    float timer = 0.0f;
    int   score = 0;
    bool  gameOver = false;

    auto isOnSnake = [&](Vector2 p) {
        for (auto& s : snake) if (s.x == p.x && s.y == p.y) return true;
        return false;
        };

    auto placeFood = [&]() {
        do {
            food = { (float)GetRandomValue(0, GRID_W - 1),
                     (float)GetRandomValue(0, GRID_H - 1) };
        } while (isOnSnake(food));
        };

    while (!WindowShouldClose())
    {
        // ---------- Ввод ----------
        if (!gameOver)
        {
            if ((IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) && dir != Dir::Down)
                nextDir = Dir::Up;
            if ((IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) && dir != Dir::Up)
                nextDir = Dir::Down;
            if ((IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) && dir != Dir::Right)
                nextDir = Dir::Left;
            if ((IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) && dir != Dir::Left)
                nextDir = Dir::Right;
        }
        else
        {
            if (IsKeyPressed(KEY_R))
            {
                snake.clear();
                snake.push_back({ GRID_W / 2,     GRID_H / 2 });
                snake.push_back({ GRID_W / 2 - 1, GRID_H / 2 });
                snake.push_back({ GRID_W / 2 - 2, GRID_H / 2 });
                dir = nextDir = Dir::Right;
                score = 0;
                timer = 0.0f;
                gameOver = false;
                placeFood();
            }
        }

        // ---------- Логика ----------
        if (!gameOver)
        {
            timer += GetFrameTime();
            if (timer >= MOVE_DELAY)
            {
                timer -= MOVE_DELAY;
                dir = nextDir;

                Vector2 head = snake.front();
                switch (dir)
                {
                case Dir::Up:    head.y -= 1; break;
                case Dir::Down:  head.y += 1; break;
                case Dir::Left:  head.x -= 1; break;
                case Dir::Right: head.x += 1; break;
                }

                // столкновение со стенами
                if (head.x < 0 || head.x >= GRID_W || head.y < 0 || head.y >= GRID_H)
                {
                    gameOver = true;
                }
                else
                {
                    // столкновение с телом (кроме последнего хвоста, который сейчас сдвинется)
                    bool hitSelf = false;
                    for (size_t i = 0; i + 1 < snake.size(); ++i)
                        if (snake[i].x == head.x && snake[i].y == head.y) { hitSelf = true; break; }

                    if (hitSelf)
                    {
                        gameOver = true;
                    }
                    else
                    {
                        snake.push_front(head);

                        if (head.x == food.x && head.y == food.y)
                        {
                            score++;
                            placeFood();
                        }
                        else
                        {
                            snake.pop_back();
                        }
                    }
                }
            }
        }

        // ---------- Рисование ----------
        BeginDrawing();
        ClearBackground(BLACK);

        // сетка (тонкая)
        for (int x = 0; x <= GRID_W; ++x)
            DrawLine(x * CELL_SIZE, 0, x * CELL_SIZE, SCREEN_H, Color{ 20, 20, 20, 255 });
        for (int y = 0; y <= GRID_H; ++y)
            DrawLine(0, y * CELL_SIZE, SCREEN_W, y * CELL_SIZE, Color{ 20, 20, 20, 255 });

        // еда
        DrawRectangle((int)(food.x * CELL_SIZE) + 1, (int)(food.y * CELL_SIZE) + 1,
            CELL_SIZE - 2, CELL_SIZE - 2, RED);

        // змейка
        for (size_t i = 0; i < snake.size(); ++i)
        {
            Color c = (i == 0) ? Color{ 0, 220, 0, 255 } : GREEN;
            DrawRectangle((int)(snake[i].x * CELL_SIZE) + 1,
                (int)(snake[i].y * CELL_SIZE) + 1,
                CELL_SIZE - 2, CELL_SIZE - 2, c);
        }

        // HUD
        DrawText(TextFormat("Score: %d", score), 10, 10, 20, RAYWHITE);

        if (gameOver)
        {
            const char* msg = "GAME OVER";
            int w = MeasureText(msg, 50);
            DrawRectangle(0, SCREEN_H / 2 - 60, SCREEN_W, 120, Fade(BLACK, 0.7f));
            DrawText(msg, (SCREEN_W - w) / 2, SCREEN_H / 2 - 40, 50, RED);
            const char* hint = "Press R to restart";
            int w2 = MeasureText(hint, 20);
            DrawText(hint, (SCREEN_W - w2) / 2, SCREEN_H / 2 + 20, 20, RAYWHITE);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}