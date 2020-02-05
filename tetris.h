#pragma once

#include <SDL.h>
#include <vector>

#include "tetromino.h"

class TetrisGame {
public:
    uint32_t userevent_game_over;
    uint32_t userevent_line_clear;
    uint32_t userevent_hard_drop;

    typedef unsigned int position_t;    // block positions
    typedef void (*gameover_cb_t)();    // gameover callback prototype

    static const unsigned int MATRIX_COLS = 10;          // matrix columns
    static const unsigned int MATRIX_ROWS = 40;          // matrix rows
    static const unsigned int MATRIX_VISIBLE_ROWS = 20;  // visible matrix rows

private:
    unsigned int level;                     // currently playing level

    Block matrix[MATRIX_ROWS][MATRIX_COLS]; // metrix

    Tetromino tetromino_current;            // current tetromino
    Tetromino tetromino_held;               // tetromino on "hold"

    std::vector<Tetromino> tetromino_bag;   // randomised list of upcoming tetrominoes
    int tetromino_row;                      // position of the currently active tetromino
    int tetromino_col;                      // position of the currently active tetromino
    void tetromino_new();                   // retrieves a new tetromino
    bool tetromino_collides();              // checks if tetromino collides with anything
    void tetromino_commit();                // commits the tetromino to the matrix
    void tetromino_bag_generate();          // generate a new tetromino bag

    gameover_cb_t gameover_cb;              // callback for gameover event

public:
    TetrisGame();
    ~TetrisGame();

    void tick();
    uint32_t tick_interval();

    Block block_at(int x, int y);
    Block block_at(int x, int y, bool include_tetromino);

    void set_gameover_cb(gameover_cb_t);

    void move_left();
    void move_right();
    void soft_drop();
    void hard_drop();
    void rotate_clockwise();
    void rotate_counterclockwise();
    
    void tetromino_hold();

    Tetromino get_held_tetromino();
    Tetromino get_next_tetromino();
};
