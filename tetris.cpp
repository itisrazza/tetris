
#include "tetris.h"

#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <random>

TetrisGame::TetrisGame()
{
    block_texture = nullptr;    // just go solid
    level = 0;

    srand(time(nullptr));
    tetromino_new();
}

TetrisGame::~TetrisGame()
{
    if (block_texture != nullptr) {
        SDL_FreeSurface(block_texture);
    }
}

void TetrisGame::tetromino_new()
{
    // generate bag if empty
    if (tetromino_bag.empty()) {
        // generate back
        for (int i = 0; i < Tetromino::tetrominoes_count; i++) {
            tetromino_bag.push_back(Tetromino::tetrominoes[i]());
        }

        // randomise list
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(tetromino_bag.begin(), tetromino_bag.end(), g);
    }

    // deque tetromino from bag
    tetromino_current = tetromino_bag.back();
    tetromino_bag.pop_back();
    
    tetromino_row = 20;
    tetromino_col = 4;

    if (tetromino_collides()) {
        // that's it game over
        status = Status::GAME_OVER;
        status_tick = -1;
    }
}

bool TetrisGame::tetromino_collides()
{
    for (position_t row = 0; row < 4; row++) {
        for (position_t col = 0; col < 4; col++) {
            // ignore blocks that aren't blocks
            if (tetromino_current.blocks[row][col].nil) continue;

            // simple boundary checks
            if (tetromino_col + col < 0 || tetromino_col + col >= MATRIX_COLS)
                return true;
            if (tetromino_row + row <= 0 || tetromino_row + row >= MATRIX_ROWS)
                return true;

            Block matrix_block = block_at(tetromino_col + col, tetromino_row + row, false);
            if (!matrix_block.nil) return true;
        }
    }
    return false;
}

void TetrisGame::tetromino_commit()
{
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            // array index check
            if (tetromino_col + col >= MATRIX_COLS) continue;
            if (tetromino_row + row >= MATRIX_ROWS) continue;

            Block tetromino_block = tetromino_current.blocks[row][col];
            if (tetromino_block.nil) continue;
            matrix[tetromino_row + row][tetromino_col + col] = tetromino_block;
        }
    }
}

uint32_t TetrisGame::tick_interval()
{
    return 1000 * SDL_pow(0.8 - ((level - 1) * 0.007), level - 1);
}

Block TetrisGame::block_at(int x, int y, bool include_tetromino)
{
    // leave it empty above
    if (y < 0 || x < 0 || x >= MATRIX_COLS || y >= MATRIX_ROWS) {
        Block block;
        block.nil = false;
        return block;
    }

    // underlying block in matrix
    Block block = matrix[y][x];

    // tetromino?
    if (include_tetromino) {
        if (y >= tetromino_row && y < tetromino_row + 4 &&
            x >= tetromino_col && x < tetromino_col + 4) {

            Block tetromino = tetromino_current.blocks[y - tetromino_row][x - tetromino_col];
            if (tetromino.nil) return block;
            return tetromino;
        }
    }

    return block;
}

Block TetrisGame::block_at(int x, int y)
{
    return block_at(x, y, true);
}

void TetrisGame::tick()
{
    // A big ball of wibbly wobbly, timey wimey stuff

    // move tetromino down
    tetromino_row--;

    // if tetromino collides
    if (tetromino_collides()) {
        tetromino_row++;
        if (tetromino_row >= MATRIX_ROWS) tetromino_row = 0;

        // commit tetromino
        tetromino_commit();

        // pick a new tetromino
        tetromino_new();
    }

    // check for line clear
    int line_check_row = 0;
    while (line_check_row < MATRIX_ROWS) {
        int block_count = 0;
        
        for (int col = 0; col < MATRIX_COLS; col++) {
            if (!block_at(col, line_check_row, false).nil) block_count++;
        }

        if (block_count == MATRIX_COLS) {
            // line cleared, copy down the line from above
            for (int copy_row = line_check_row; copy_row < MATRIX_ROWS - 1; copy_row++) {
                for (int copy_col = 0; copy_col < MATRIX_COLS - 1; copy_col++) {
                    matrix[copy_row][copy_col] = matrix[copy_row + 1][copy_col];
                }
            }
        } else {
            line_check_row++;
        }
    }
}

void TetrisGame::move_left()
{
    tetromino_col--;
    if (tetromino_collides()) tetromino_col++;
}

void TetrisGame::move_right()
{
    tetromino_col++;
    if (tetromino_collides()) tetromino_col--;
}

void TetrisGame::soft_drop()
{
    tetromino_row--;
    
    // if tetromino collides
    if (tetromino_collides()) {
        tetromino_row++;
        if (tetromino_row >= MATRIX_ROWS) tetromino_row = 0;

        // commit tetromino
        tetromino_commit();

        // pick a new tetromino
        tetromino_new();
    }
}

void TetrisGame::hard_drop()
{
    while (!tetromino_collides()) {
        tetromino_row--;
    }

    tetromino_row++;
    if (tetromino_row >= MATRIX_ROWS) tetromino_row = 0;

    tetromino_commit();
    tetromino_new();
}

void TetrisGame::rotate_clockwise()
{
    // keep a copy of the original tetromino
    auto original = tetromino_current;
    tetromino_current = Tetromino();
    tetromino_current.nil = false;
    tetromino_current.size = original.size;

    for (int y = 0; y < original.size; y++) {
        for (int x = 0; x < original.size; x++) {
            tetromino_current.blocks[x][original.size - y - 1] = original.blocks[y][x];
        }
    }

    if (tetromino_collides()) rotate_counterclockwise();
}

void TetrisGame::rotate_counterclockwise()
{
    // keep a copy of the original tetromino
    auto original = tetromino_current;
    tetromino_current = Tetromino();
    tetromino_current.nil = false;
    tetromino_current.size = original.size;

    for (int y = 0; y < original.size; y++) {
        for (int x = 0; x < original.size; x++) {
            tetromino_current.blocks[original.size - x - 1][y] = original.blocks[y][x];
        }
    }

    if (tetromino_collides()) rotate_clockwise();

}
