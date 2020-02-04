#pragma once

#include <SDL.h>
#include <vector>

#include "tetromino.h"

class TetrisGame {
public:
    enum class Status {
        NORMAL_PLAY,
        LINE_CLEAR,
        GAME_OVER
    };

    Status status;
    int status_tick;

    typedef unsigned int position_t;

    static const unsigned int MATRIX_COLS = 10;          // matrix columns
    static const unsigned int MATRIX_ROWS = 40;          // matrix rows
    static const unsigned int MATRIX_VISIBLE_ROWS = 20;  // visible matrix rows

    static const uint32_t level_intervals[];

private:
    SDL_Surface* block_texture;

    unsigned int level;

    Block matrix[MATRIX_ROWS][MATRIX_COLS];

    Tetromino tetromino_current;
    std::vector<Tetromino> tetromino_bag;
    int tetromino_row;
    int tetromino_col;
    void tetromino_new();
    bool tetromino_collides();
    void tetromino_commit();

public:
    TetrisGame();
    ~TetrisGame();

    void tick();
    uint32_t tick_interval();

    Block block_at(int x, int y);
    Block block_at(int x, int y, bool include_tetromino);

    void move_left();
    void move_right();
    void soft_drop();
    void hard_drop();
    void rotate_clockwise();
    void rotate_counterclockwise();
};
