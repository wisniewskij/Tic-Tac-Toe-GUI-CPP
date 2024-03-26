#include <iostream>
#include "raylib.h"
#include "game_state.h"

#define GRID_SIZE 5
#define WINNING_SIZE 4
#define MARGIN 65
#define CELL_SIZE 100
#define GAP_SIZE 22

int main() {

    Color purple = {71, 68, 157, 255};

    int full_height = 2*MARGIN + CELL_SIZE * GRID_SIZE + (GRID_SIZE - 1) * GAP_SIZE;

    InitWindow(full_height, full_height, "Tic Tac Toe");
    SetTargetFPS(60);

    Game_state game_state = Game_state(GRID_SIZE, WINNING_SIZE, CELL_SIZE, MARGIN, GAP_SIZE);


    Image image = LoadImage("resources/restart.png");
    ImageResize(&image, image.width/2, image.height/2);
    Texture2D restart_button = LoadTextureFromImage(image);

    float frameHeight = (float)restart_button.height;
    Rectangle source_rectangle = { 0, 0, (float)restart_button.width, frameHeight };
    Rectangle button_bounds = { full_height/2.0f - restart_button.width/2.0f, full_height/2.0f - restart_button.height/2.0f + CELL_SIZE, (float)restart_button.width, frameHeight };

    while(!WindowShouldClose()) {
        Vector2 mousePos = GetMousePosition();

        if (game_state.is_game_won() && GetTime() - game_state.get_win_time() >= 0.2 && CheckCollisionPointRec(mousePos, button_bounds))
        {
            if ( IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                game_state.restart_game();
            }
        }

        BeginDrawing();
        ClearBackground(purple);
        game_state.draw();

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {

            int gridX = static_cast<int>((mousePos.x - MARGIN) / (CELL_SIZE + GAP_SIZE));
            int gridY = static_cast<int>((mousePos.y - MARGIN) / (CELL_SIZE + GAP_SIZE));

            if(mousePos.x <= MARGIN + CELL_SIZE * (gridX + 1) + gridX * GAP_SIZE
                && mousePos.x >= MARGIN + CELL_SIZE * gridX + std::max(0, gridX-1) * GAP_SIZE
                && mousePos.y >= MARGIN + CELL_SIZE * gridY + std::max(0, gridY-1) * GAP_SIZE
                && mousePos.y <= MARGIN + CELL_SIZE * (gridY + 1) + gridY * GAP_SIZE
                && gridX < GRID_SIZE && gridY < GRID_SIZE)
                    game_state.move(gridX, gridY);
        }
        game_state.if_won_draw_state();

        if(game_state.is_game_won())
            DrawTextureRec(restart_button, source_rectangle, (Vector2){ button_bounds.x, button_bounds.y }, WHITE); // Draw restart_button frame

        EndDrawing();
    }
    CloseWindow();
    UnloadTexture(restart_button);
    return 0;
}
