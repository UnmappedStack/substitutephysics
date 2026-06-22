#include "raylib.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define WIDTH  800
#define HEIGHT 450

#define SPEED_MAX 15
#define SPEED_MIN  0

typedef struct {
    float from, to;
} Range;

typedef enum {
    GAME_RUNNING,
    GAME_DEAD,
    GAME_PAUSED,
} GameState;

#define VECTOR(x, y) ((Vector2){x, y})
#define MAP(value, old_min, old_max, new_min, new_max) (((value - old_min) / (old_max - old_min)) * (new_max - new_min) + new_min)

#define SET_STARTING_VALS() \
    do { \
        game_state = GAME_RUNNING; \
        level_length = 5.0; \
        secs_to_next_level = level_length; \
        speed_range = (Range) { .from=5, .to=8 }; \
        velocity = 1; \
        level = 1; \
	time_out_of_bounds = 0; \
    } while (0)

int main(void) {
    InitWindow(WIDTH, HEIGHT, "Physics substitute");
    SetTargetFPS(60);

    Image spinners_img[] = {
        LoadImage("assets/clock.png"),
        LoadImage("assets/earth.png"),
        LoadImage("assets/fan.png"),
        LoadImage("assets/spintowin.png"),
        LoadImage("assets/wheel.png"),
    };
    for (int i = 0; i < sizeof(spinners_img)/sizeof(spinners_img[0]); i++)
        ImageResize(&spinners_img[i], spinners_img[i].width/2, spinners_img[i].height/2);
    Texture2D spinners[] = {
        {}, {}, {}, {}, {},
    };
    const char *labels[] = {"Clock", "Earth", "Fan", "Spin-to-win wheel", "Wheel"};
    for (int i = 0; i < sizeof(spinners_img)/sizeof(spinners_img[0]); i++)
        spinners[i] = LoadTextureFromImage(spinners_img[i]);

    // these are for DrawTexturePro to rotate it around its centre
    int frame_width  = spinners[0].width;
    int frame_height = spinners[0].height;
    Rectangle source_rect = { 0.0f, 0.0f, (float)frame_width, (float)frame_height };
    Rectangle dest_rect   = { WIDTH/2.0f, HEIGHT/2.0f, frame_width*2.0f, frame_height*2.0f };
    Vector2 origin        = { (float)frame_width, (float)frame_height};


    int spinner_idx = 0;
    Texture2D speedometre = LoadTexture("assets/speedometre.png");
    
    float angle = 0;
    const char *right_text = NULL;
    struct Color goal_colour = RED;

    GameState game_state;
    float secs_to_next_level, level_length, time_out_of_bounds, velocity;
    int level;
    Range speed_range;
    SET_STARTING_VALS();

    float message_timeout = 0;
    const char *message = NULL;

    int speedometre_level = 0;
    while (!WindowShouldClose()) {
        int speedometre_y = HEIGHT/2-speedometre.height/2;
        int speedometre_x = WIDTH-speedometre.width-10;
        if ((IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) &&
	    speedometre_level < (speedometre_y+speedometre.height-5)) velocity += 0.05;
        else if ((IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) &&
	    velocity >= SPEED_MIN) velocity -= 0.05;
        else if (velocity > 0.01) velocity -= 0.025;
        BeginDrawing();
        ClearBackground(BLACK);
	switch (game_state) {
	case GAME_DEAD:
            message = TextFormat("you died (level %i)", level);
	    if (GetKeyPressed()) {
	        SET_STARTING_VALS();
	    }
            DrawText(message, (WIDTH/2)-(MeasureText(message, 50)/2), (HEIGHT/2)-25, 50, RAYWHITE);
	    message = "<press any key to start again>";
            DrawText(message, (WIDTH/2)-(MeasureText(message, 30)/2), (HEIGHT/2)+25, 30, RAYWHITE);
            EndDrawing();
            continue;
	case GAME_PAUSED:
	    if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_SPACE)) game_state = GAME_RUNNING;
	    else if (GetKeyPressed()) game_state = GAME_MENU;
	    message = "game paused";
            DrawText(message, (WIDTH/2)-(MeasureText(message, 50)/2), (HEIGHT/2)-50, 50, RAYWHITE);
	    message = "esc or space to continue, any other key to quit";
            DrawText(message, (WIDTH/2)-(MeasureText(message, 30)/2), (HEIGHT/2)+15, 30, RAYWHITE);
	    message = "CONTROLS:\n"
		      "w/s or up/down arrows to increase/decrease speed\n"
		      "space to pause\n";
            DrawText(message, 5, HEIGHT-(20*3)-5, 20, RAYWHITE);
	    EndDrawing();
	    continue;
	case GAME_RUNNING:
	    break;
	}
        if (IsKeyPressed(KEY_SPACE)) game_state = GAME_PAUSED;
        DrawTexture(speedometre, speedometre_x, speedometre_y, RAYWHITE);
        // idk how the following line works exactly i just kept changing the mapping until it seemed about right
        speedometre_level = MAP(velocity, 0, (speed_range.from + speed_range.to)/2+speed_range.to, speedometre_y, speedometre_y + speedometre.height);
	if (speedometre_level > (speedometre_y+speedometre.height)) {
	    velocity = 1;
            speedometre_level = MAP(velocity, 0, (speed_range.from + speed_range.to)/2+speed_range.to, speedometre_y, speedometre_y + speedometre.height);
	}
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
            secs_to_next_level -= GetFrameTime();
            if (secs_to_next_level <= 0) {
                level_length *= 1.2;
                secs_to_next_level = level_length;
                float diff = speed_range.to - speed_range.from;
                speed_range.from = rand() % 11 + 1;
                speed_range.to = speed_range.from + (diff*0.9);
                message = "nice u did it, next level";
                level++;
                spinner_idx++;
                if (spinner_idx >= 5) spinner_idx = 0;
                message_timeout = 100;
            }
        } else {
            bool above = velocity > speed_range.to;
            time_out_of_bounds += GetFrameTime();
            right_text = TextFormat("out of bounds for %.1f seconds\n(2 max)", time_out_of_bounds);
            if (time_out_of_bounds >= 2) game_state = GAME_DEAD;
            DrawText(right_text, WIDTH-MeasureText(right_text, 20), 0, 20, RED);
            secs_to_next_level = 5;
        }
        DrawTexturePro(spinners[spinner_idx], source_rect, dest_rect, origin, angle, RAYWHITE);
        DrawText(labels[spinner_idx], (WIDTH/2)-(MeasureText(labels[spinner_idx], 40)/2), 30, 40, RAYWHITE);
        EndDrawing();
        angle += velocity;
        if (message_timeout != 0.0) {
            message_timeout -= 1;
            DrawText(message, (WIDTH/2)-(MeasureText(message, 50)/2), HEIGHT-55, 50, RAYWHITE);
        }
    }

    CloseWindow();

    return 0;
}
