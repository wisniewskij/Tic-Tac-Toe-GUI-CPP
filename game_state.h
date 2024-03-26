#pragma once
#ifndef CPP_PROJECT_GAME_STATE_H
#define CPP_PROJECT_GAME_STATE_H

#include "raylib.h"

class Game_state {
    int turn_no;
    static constexpr int PLAYER_ONE = 1, PLAYER_TWO = 2, EMPTY = 0;
    int **grid, grid_size, consecutive_to_win, cell_size, margin, gap_size;
    Font default_font;
    bool won, drawn;
    Vector2 winline_lt, winline_rb;
    double win_time;

public:
    explicit Game_state(int grid_size = 3, int consecutive_to_win = 3, int cell_size = 50, int margin = 20, int gap_size = 5);
    ~Game_state();
    [[nodiscard]] int which_player() const;
    void draw() const;
    void move(int x, int y);
    void check_for_win();
    void win(int player, Vector2 lt, Vector2 rb);
    void if_won_draw_state();
    bool is_game_won() const;
    bool is_game_drawn() const;
    void restart_game();
    double get_win_time();
};


#endif //CPP_PROJECT_GAME_STATE_H
