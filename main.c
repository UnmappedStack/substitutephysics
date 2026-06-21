#include "raylib.h"
#include <stdio.h>
#include <poll.h>
#include <time.h>
#include <stdlib.h>

#define WIDTH  800
#define HEIGHT 450

#define SPEED_MAX 15
#define SPEED_MIN  0

typedef struct {
    float from, to;
} Range;

#define VECTOR(x, y) ((Vector2){x, y})
#define MAP(value, old_min, old_max, new_min, new_max) (((value - old_min) / (old_max - old_min)) * (new_max - new_min) + new_min)

int main(void) {
    struct pollfd pfd[1];
    InitWindow(WIDTH, HEIGHT, "Physics substitute");

    Texture2D speedometre = LoadTexture("speedometre.png");
    
    int level = 1;
    float angle = 0;
    float velocity = 1;
    Range speed_range = { .from=5, .to=8 };
    const char *right_text = NULL;
    struct Color goal_colour = RED;
    float time_out_of_bounds = 0.0;
    float level_length = 5.0;
    float secs_to_next_level = level_length;
    bool game_running = true;

    float message_timeout = 0;
    const char *message = NULL;

    while (!WindowShouldClose()) {
        if (IsKeyDown(KEY_W) && velocity < SPEED_MAX) velocity += 0.05;
        else if (IsKeyDown(KEY_S) && velocity >= SPEED_MIN) velocity -= 0.05;
        else if (velocity > 0.01) velocity -= 0.025;
        BeginDrawing();
        ClearBackground(BLACK);
        if (!game_running) {
            message = "you died";
            DrawText(message, (WIDTH/2)-(MeasureText(message, 50)/2), (HEIGHT/2)-25, 50, RAYWHITE);
            EndDrawing();
            continue;
        }
        int speedometre_y = HEIGHT/2-speedometre.height/2;
        int speedometre_x = WIDTH-speedometre.width-10;
        DrawTexture(speedometre, speedometre_x, speedometre_y, RAYWHITE);
        // idk how the following line works exactly i just kept changing the mapping until it seemed about right
        int speedometre_level = MAP(velocity, 0, (speed_range.from+ speed_range.to)/2+speed_range.to, speedometre_y, speedometre_y + speedometre.height);
        DrawTriangle(VECTOR(speedometre_x, speedometre_level+5),
                     VECTOR(speedometre_x, speedometre_level+15),
                     VECTOR(speedometre_x+10, speedometre_level+10),
                     RAYWHITE); 
        DrawText(TextFormat("level %i", level), 0, 0, 20, LIGHTGRAY);
        if (velocity >= speed_range.from && velocity <= speed_range.to) {
            right_text = TextFormat("%.1f seconds to next level", secs_to_next_level);
            DrawText(right_text, WIDTH-MeasureText(right_text, 20), 0, 20, GREEN);
            goal_colour = GREEN;
            time_out_of_bounds = 0;
            secs_to_next_level -= 0.01;
            if (secs_to_next_level <= 0) {
                level_length *= 1.2;
                secs_to_next_level = level_length;
                float diff = speed_range.to - speed_range.from;
                speed_range.from = rand() % 11 + 1;
                speed_range.to = speed_range.from + (diff*0.9);
                message = "nice u did it, next level";
                level++;
                message_timeout = 100;
            }
        } else {
            bool above = velocity > speed_range.to;
            time_out_of_bounds += 0.01;
            right_text = TextFormat("out of bounds for %.1f seconds\n(2 max)", time_out_of_bounds);
            if (time_out_of_bounds >= 2) game_running = false;
            DrawText(right_text, WIDTH-MeasureText(right_text, 20), 0, 20, RED);
            secs_to_next_level = 5;
        }
        DrawCircleSector((Vector2) {400, 225}, 100, angle, angle+45, 1, RAYWHITE); // TODO: make this a picture
        EndDrawing();
        angle += velocity;
        if (message_timeout != 0.0) {
            message_timeout -= 1;
            DrawText(message, (WIDTH/2)-(MeasureText(message, 50)/2), (HEIGHT/2)-25, 50, RAYWHITE);
        }
        poll(pfd, 1, 10);
    }

    CloseWindow();

    return 0;
}
