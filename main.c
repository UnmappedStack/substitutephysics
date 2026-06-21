#include "raylib.h"
#include <poll.h>
#include <time.h>
#include <stdlib.h>

#define WIDTH  800
#define HEIGHT 450

typedef struct {
    float from, to;
} Range;

int main(void) {
    struct pollfd pfd[1];
    InitWindow(WIDTH, HEIGHT, "Physics substitute");
    float angle = 0;
    float velocity = 1;
    Range speed_range = { .from=5, .to=8 };
    const char *goal_text = NULL;
    const char *right_text = NULL;
    struct Color goal_colour = RED;
    float time_out_of_bounds = 0.0;
    float level_length = 5.0;
    float secs_to_next_level = level_length;
    while (!WindowShouldClose()) {
        if (IsKeyDown(KEY_W)) velocity += 0.05;
        else if (IsKeyDown(KEY_S)) velocity -= 0.05;
        else if (velocity > 0.01) velocity -= 0.025;
        BeginDrawing();
        ClearBackground(BLACK);
        DrawText("be careful, this game is pretty serious", 0, 0, 20, LIGHTGRAY);
        DrawText(TextFormat("speed: %.1f", velocity), 0, 25, 20, LIGHTGRAY);
        if (velocity >= speed_range.from && velocity <= speed_range.to) {
            goal_text = TextFormat("goal: %.1f-%.1f (within goal)", speed_range.from, speed_range.to);
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
            }
        } else {
            bool above = velocity > speed_range.to;
            goal_text = TextFormat("goal: %.1f-%.1f (outside goal!! %s)",
                    speed_range.from, speed_range.to, (above) ? "slow down" : "speed up");
            goal_colour = RED;
            time_out_of_bounds += 0.01;
            right_text = TextFormat("out of bounds for %.1f seconds\n(2 max)", time_out_of_bounds);
            DrawText(right_text, WIDTH-MeasureText(right_text, 20), 0, 20, RED);
            secs_to_next_level = 5;
        }
        DrawText(goal_text, 0, 50, 20, goal_colour);
        DrawCircleSector((Vector2) {400, 225}, 100, angle, angle+45, 1, RAYWHITE); // TODO: make this a picture
        EndDrawing();
        angle += velocity;
        poll(pfd, 1, 10);
    }

    CloseWindow();

    return 0;
}
