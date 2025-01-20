#include "raylib.h"
#include "raymath.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define CELL_SIZE 50
#define CELL_COUNT 20

#define SNAKE_PART_SIZE CELL_SIZE
#define SNAKE_PART_PADDING 10
#define SNAKE_MAX_LENGTH 256

#define SCREEN_WIDTH (CELL_SIZE * CELL_COUNT)
#define SCREEN_HEIGHT (CELL_SIZE * CELL_COUNT)

#define FRAME_RATE 60
#define SNAKE_MOVEMENT_DELAY 0.5

typedef struct Snake {
  Vector2 body[SNAKE_MAX_LENGTH];
  Vector2 head_direction;
  int length;
} Snake;

void DrawSnake(Snake *snake) {
  Vector2 position = snake->body[0];
  for (int i = 0; i < snake->length; i++) {
    Vector2 position = snake->body[i];
    DrawRectangle(position.x, position.y, SNAKE_PART_SIZE - SNAKE_PART_PADDING,
                  SNAKE_PART_SIZE - SNAKE_PART_PADDING, RED);
  }
  DrawText("HEAD", position.x, position.y, 16, WHITE);
}

void GrowSnake(Snake *snake) {
  assert(snake->length < SNAKE_MAX_LENGTH);
  Vector2 first_cell;

  if (snake->length > 0) {
    first_cell = snake->body[0];
  } else {
    first_cell = Vector2Zero();
  }

  Vector2 new_cell = Vector2Subtract(
      first_cell, Vector2Scale(snake->head_direction, SNAKE_PART_SIZE));

  for (int i = snake->length; i > 0; i--) {
    snake->body[i] = snake->body[i - 1];
  }

  snake->body[0] = new_cell;
  snake->length += 1;
}

void MoveSnake(Snake *snake, float delta) {

  for (int i = snake->length; i > 0; i--) {
    snake->body[i] = snake->body[i - 1];
  }

  Vector2 first_cell = snake->body[0];

  Vector2 new_first_cell = Vector2Subtract(
      first_cell, Vector2Scale(snake->head_direction, SNAKE_PART_SIZE));

  snake->body[0] = new_first_cell;
}

Vector2 GenerateFood() {
  int x = rand() % CELL_COUNT;
  int y = rand() % CELL_COUNT;

  return Vector2Scale((Vector2){.x = x, .y = y}, CELL_SIZE);
}

Snake CreateSnake() {
  Vector2 center = {.x = SCREEN_WIDTH / 2.0, .y = SCREEN_HEIGHT / 2.0};

  Snake snake = {0};
  snake.head_direction.x -= 1;
  snake.length = 1;
  snake.body[0] = center;
  GrowSnake(&snake);
  GrowSnake(&snake);
  GrowSnake(&snake);

  return snake;
}

int main() {

  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Snake");
  SetTargetFPS(FRAME_RATE);
  PollInputEvents();

  Vector2 center = {.x = SCREEN_WIDTH / 2.0, .y = SCREEN_HEIGHT / 2.0};

  Snake snake = CreateSnake();
  float last_moved = 0.0;
  Vector2 food = GenerateFood();
  bool is_game_over = false;
  int score = 0;

  while (!WindowShouldClose()) {
    float delta = GetFrameTime();
    BeginDrawing();

    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, BLACK);
    if (is_game_over) {
      DrawText("GAME OVER, Press 'R' to Restart", 50, center.y, 32, RED);

      if (IsKeyPressed(KEY_R)) {
        snake = CreateSnake();
        score = 0;
        last_moved = 0;
        is_game_over = false;
        food = GenerateFood();
      }
      
    } else {

      last_moved += delta;

      if (snake.head_direction.y == 0) {
        if (IsKeyPressed(KEY_W)) {
          snake.head_direction.y = 1;
          snake.head_direction.x = 0;
        } else if (IsKeyPressed(KEY_S)) {
          snake.head_direction.y = -1;
          snake.head_direction.x = 0;
        }
      } else if (snake.head_direction.x == 0) {
        if (IsKeyPressed(KEY_A)) {
          snake.head_direction.y = 0;
          snake.head_direction.x = 1;
        } else if (IsKeyPressed(KEY_D)) {
          snake.head_direction.y = 0;
          snake.head_direction.x = -1;
        }
      }

      if (last_moved > SNAKE_MOVEMENT_DELAY) {
        MoveSnake(&snake, delta);
        last_moved = 0;
      }

      // if (IsKeyPressed(KEY_G)) {
      //   GrowSnake(&snake);
      // }

      Vector2 head = snake.body[0];

      if (Vector2Equals(head, food)) {
        TraceLog(LOG_INFO, "Snake Ate Food");
        GrowSnake(&snake);
        head = snake.body[0];
        score += 1;
        food = GenerateFood();
      }

      for (int i = 1; i < snake.length; i++) {
        if (Vector2Equals(snake.body[i], head)) {
          TraceLog(LOG_INFO, "Snake Bit Itself of its %d part", i);
          is_game_over = true;
        }
      }

      if (head.x <= 0 || head.y <= 0 || head.x >= SCREEN_WIDTH || head.y >= SCREEN_HEIGHT) {
        TraceLog(LOG_INFO, "Snake Went Out");
        is_game_over = true;
      }

      
    }

    char score_text[256] = {};
    sprintf(score_text, "Score: %d", score);

    DrawText(score_text, 10, 10, 32, WHITE);

    DrawRectangle((int)food.x, (int)food.y, CELL_SIZE, CELL_SIZE, GREEN);
    DrawSnake(&snake);

    EndDrawing();
  }

  return 0;
}
