#pragma once

#include <SDL.h>
#include <vector>

#include "mode.h"
#include "tetromino.h"

class TetrisMode : public GameMode {
    static const unsigned int BLOCK_SIZE = 16;

    static const unsigned int MATRIX_COLS = 10;         // matrix columns
    static const unsigned int MATRIX_ROWS = 40;         // matrix rows
    static const unsigned int MATRIX_VISIBLE_ROWS = 20; // visible matrix rows
    Block matrix[MATRIX_ROWS][MATRIX_COLS];             // metrix

    SDL_TimerID tick_timer_id = 0;

    unsigned int level = 1;                 // currently playing level
    unsigned int score = 0;                 // score
    unsigned int lines = 0;                 // lines cleared

    Tetromino tetromino_current;            // current tetromino
    Tetromino tetromino_held;               // tetromino on "hold"

    std::vector<Tetromino> tetromino_bag;   // randomised list of upcoming tetrominoes
    int tetromino_row;                      // position of the currently active tetromino
    int tetromino_col;                      // position of the currently active tetromino
    void tetromino_new();                   // retrieves a new tetromino
    void tetromino_new(bool deque);         // reset tetromino without deque
    bool tetromino_collides();              // checks if tetromino collides with anything
    void tetromino_commit();                // commits the tetromino to the matrix
    void tetromino_bag_generate();          // generate a new tetromino bag

    Block block_at(int x, int y);
    Block block_at(int x, int y, bool include_tetromino);

    void move_left();
    void move_right();
    void soft_drop();
    void hard_drop();
    void rotate_clockwise();
    void rotate_counterclockwise();
    void tetromino_hold();

    SDL_Surface* render_tetromino(Tetromino tetromino);
    SDL_Surface* render_matrix();

    Tetromino get_held_tetromino();
    Tetromino get_next_tetromino();

    void tick_game();
    void tick_gameover();

    struct color { char r; char g; char b; };

    enum class TetrominoSource {
        BAG,
        HOLD
    };
    TetrominoSource tetromino_source = TetrominoSource::BAG;

    enum class State {
        PLAYING,
        GAME_OVER
    };
public:
    State state;
private:
    int gameover_row = 0;

public:
    TetrisMode();
    ~TetrisMode();

    void initialise();
    void deinitialise();

    void draw(SDL_Surface*);
    void handle_event(SDL_Event&);

    void tick();
    uint32_t tick_interval();
};