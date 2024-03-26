//
// Created by 48514 on 3/1/2024.
//

#include <algorithm>
#include "game_state.h"
#include "raylib.h"
#include <iostream>
#include <string>

int Game_state::which_player() const {
    if(turn_no&1) return PLAYER_ONE;
    return PLAYER_TWO;
}

Game_state::Game_state(int grid_size, int consecutive_to_win, int cell_size, int margin, int gap_size) {
    turn_no = 1;
    this->grid_size = grid_size;
    this->consecutive_to_win = consecutive_to_win;
    this->cell_size = cell_size;
    this->margin = margin;
    this->gap_size = gap_size;
    default_font = LoadFontEx("resources/ARCADECLASSIC.TTF", 64, 0, 0);
    won = false;
    drawn = false;
    win_time = -1;

    grid = new int*[grid_size];
    for(int i=0;i<grid_size;i++)
        grid[i] = new int[grid_size];

    for(int i=0;i<grid_size;i++)
        for(int j=0;j<grid_size;j++)
            grid[i][j] = EMPTY;
}

Game_state::~Game_state() {
    UnloadFont(default_font);
    for(int i=0;i<grid_size;i++)
        delete[] grid[i];
    delete[] grid;
}

void Game_state::draw() const {
    for(int i=0;i<grid_size;i++)
        for(int j=0;j<grid_size;j++) {
            DrawRectangle(margin+j*cell_size+j * gap_size,margin+i*cell_size+i*gap_size,cell_size,cell_size,Color{23,31,89,255});
            if(grid[i][j] == PLAYER_ONE) {
                Vector2 center = {float(margin+(j + 0.5)*cell_size+j * gap_size), float(margin+(i + 0.5)*cell_size+i*gap_size)};
                DrawRing(center, cell_size * 0.3, cell_size * 0.4, 0, 360, 0, {81,118,167, 255});
            } else if(grid[i][j] == PLAYER_TWO) {
                Vector2 start1 = {float(margin+(j+0.2)*cell_size+j * gap_size), float(margin+(i+0.2)*cell_size+i*gap_size)},
                end1 = {float(margin+(j+0.8)*cell_size+j * gap_size), float(margin+(i+0.8)*cell_size+i*gap_size)},
                start2 = {float(margin+(j+0.2)*cell_size+j * gap_size), float(margin+(i+0.8)*cell_size+i*gap_size)},
                end2 = {float(margin+(j+0.8)*cell_size+j * gap_size), float(margin+(i+0.2)*cell_size+i*gap_size)};

                DrawLineEx(start1, end1, 10, {26,85,171,255});
                DrawLineEx(start2, end2, 10, {26,85,171,255});
            }
        }

    std::string round_text = "Round " + std::to_string(turn_no);
    Vector2 round_text_offset = MeasureTextEx(default_font, round_text.c_str(), 32, 2);
    std::string player_text = "Player " + std::string((which_player() == 1? "O" : "X"));
    Vector2 player_text_offset =  MeasureTextEx(default_font, player_text.c_str(), 32, 2);



    DrawTextEx(default_font, round_text.c_str(), {float(margin), float(margin)/2-round_text_offset.y/2}, 32, 2, WHITE);
    DrawTextEx(default_font, player_text.c_str(), {float(margin + cell_size * grid_size + (grid_size - 1) * gap_size) - player_text_offset.x, float(margin)/2-round_text_offset.y/2}, 32, 2, WHITE);
}

void Game_state::move(int x, int y) {
    if(is_game_won()) return;
    if(grid[y][x] != EMPTY) return;

    grid[y][x] = which_player();
    check_for_win();
    if(!is_game_won())
        turn_no++;
}

void Game_state::check_for_win() {
    if(is_game_won()) return;

    int y_progress[] = {0, 1, 1, 1}, x_progress[] = {1, 0, 1, -1}; //horizontal, vertical, diagonal1, diagonal2
    for(int i=0;i<grid_size;i++)
        for(int j=0;j<grid_size;j++)
            for(int k=0;k<4;k++)
                if(i+y_progress[k]*consecutive_to_win - 1 < grid_size && j+x_progress[k]*consecutive_to_win - 1 < grid_size && j+x_progress[k]*consecutive_to_win + 1 >= 0) {
                    int player_one_counter = 0, empty_counter = 0;
                    for(int l=0;l<consecutive_to_win;l++)
                        if(grid[i+y_progress[k]*l][j+x_progress[k]*l] == PLAYER_ONE) player_one_counter++;
                        else if(grid[i+y_progress[k]*l][j+x_progress[k]*l] == EMPTY) empty_counter++;
                    if(player_one_counter%consecutive_to_win == 0 && empty_counter == 0) {
                        win(player_one_counter>0 ? PLAYER_ONE : PLAYER_TWO, {float(j), float(i)}, {float(j+x_progress[k]*(consecutive_to_win-1)), float(i+y_progress[k]*(consecutive_to_win-1))});
                        return;
                    }
                }

    if(turn_no == grid_size * grid_size) {
        win(EMPTY, {0, 0}, {0, 0});
        drawn = true;
    }
}

void Game_state::win(int player, Vector2 lt, Vector2 rb) {
    winline_lt = lt;
    winline_rb = rb;

    won = true;
    win_time = GetTime();
}

void Game_state::if_won_draw_state() {
    if(!is_game_won()) return;

    Vector2 start = {float((winline_lt.x + 0.5) * cell_size + margin + winline_lt.x  * gap_size), float((winline_lt.y + 0.5) * cell_size + margin + winline_lt.y * gap_size)},
    end = {float((winline_rb.x + 0.5) * cell_size + margin + winline_rb.x * gap_size), float((winline_rb.y + 0.5) * cell_size + margin + winline_rb.y * gap_size)};
    DrawLineEx(start, end, 26, RED);

    std::string win_text;
    if(is_game_drawn())
        win_text = "Game drawn!";
    else
        win_text = "Player " + std::to_string(which_player()) + " wins!";
    Vector2 win_text_offset = MeasureTextEx(default_font, win_text.c_str(), 64, 2);
    Vector2 win_text_pos = {float(2*margin + cell_size * grid_size + (grid_size - 1) * gap_size - win_text_offset.x)/2, float(2*margin + cell_size * grid_size + (grid_size - 1) * gap_size - win_text_offset.y)/2};
    float outline_size = 4;

    DrawTextEx(default_font, win_text.c_str(), {win_text_pos.x - outline_size, win_text_pos.y - outline_size}, 64, 2, BLACK);
    DrawTextEx(default_font, win_text.c_str(), {win_text_pos.x + outline_size, win_text_pos.y - outline_size}, 64, 2, BLACK);
    DrawTextEx(default_font, win_text.c_str(), {win_text_pos.x - outline_size, win_text_pos.y + outline_size}, 64, 2, BLACK);
    DrawTextEx(default_font, win_text.c_str(), {win_text_pos.x + outline_size, win_text_pos.y + outline_size}, 64, 2, BLACK);
    DrawTextEx(default_font, win_text.c_str(), win_text_pos, 64, 2, WHITE);
}

bool Game_state::is_game_won() const {
    return won;
}

void Game_state::restart_game() {
    turn_no = 1;
    won = false;
    drawn = false;
    win_time = -1;

    for(int i=0;i<grid_size;i++)
        for(int j=0;j<grid_size;j++)
            grid[i][j] = EMPTY;
}

double Game_state::get_win_time() {
    return win_time;
}

bool Game_state::is_game_drawn() const {
    return drawn;
}


