
#include "tetris.h"

#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <exception>
#include <system_error>
#include <random>

#include <SDL.h>
#include <SDL_ttf.h>

TetrisMode::TetrisMode()
{
    level = 0;

    srand(time(nullptr));
    tetromino_bag_generate();
    tetromino_new();

    tick_timer_id = SDL_AddTimer(1000, [](uint32_t interval, void* userdata) -> uint32_t {
        TetrisMode* tetris_mode = (TetrisMode*)userdata;
        tetris_mode->tick();
        return 1000;
    }, this);

    // userevent_game_over = SDL_RegisterEvents(3);
    // if (userevent_game_over == (uint32_t) -1) {
    //     throw std::runtime_error("Failed to register events.");
    // }

    // userevent_hard_drop = userevent_game_over + 1;
    // userevent_line_clear = userevent_hard_drop + 1;
}

TetrisMode::~TetrisMode()
{
}

void TetrisMode::initialise()
{

}

void TetrisMode::deinitialise()
{

}

void TetrisMode::draw(SDL_Surface* surface)
{   
    auto font = game_system->get_font();

    // draw the matrix on-screen
    SDL_Surface* matrix_surface = render_matrix();
    SDL_Rect rect = { surface->w / 2 - matrix_surface->w / 2,
                      surface->h / 2 - matrix_surface->h / 2,
                      matrix_surface->w, matrix_surface->h };

    // draw border
    rect.x -= 1;
    rect.y -= 1;
    rect.w += 2;
    rect.h += 2;
    SDL_FillRect(surface, &rect, SDL_MapRGB(surface->format, 255, 255, 255));
    rect.x += 1;
    rect.y += 1;
    rect.w -= 2;
    rect.h -= 2;

    // copy the matrix over and free it
    SDL_BlitSurface(matrix_surface, nullptr, surface, &rect);
    SDL_FreeSurface(matrix_surface);

    // test
    SDL_Color color = { 255, 255, 255, SDL_ALPHA_OPAQUE };
    SDL_Surface* next_text = TTF_RenderUTF8_Solid(font, "NEXT", color);
    rect.x = 51 * 8;
    rect.y = 32;
    rect.w = next_text->w;
    rect.h = next_text->h;
    SDL_BlitSurface(next_text, nullptr, surface, &rect);
    SDL_FreeSurface(next_text);
    auto next_tetromino = render_tetromino(get_next_tetromino());
    rect.y += 16;
    rect.w = next_tetromino->w;
    rect.h = next_tetromino->h;
    SDL_BlitSurface(next_tetromino, nullptr, surface, &rect);
    SDL_FreeSurface(next_tetromino);

    // hold
    SDL_Surface* hold_text = TTF_RenderUTF8_Solid(font, "HOLD", color);
    rect.w = hold_text->w;
    rect.h = hold_text->h;
    rect.x = 29 * 8 - rect.w;
    rect.y = 32;
    SDL_BlitSurface(hold_text, nullptr, surface, &rect);
    SDL_FreeSurface(hold_text);
    auto hold_tetromino = render_tetromino(get_held_tetromino());
    rect.x += rect.w - hold_tetromino->w;
    rect.y += 16;
    rect.w = hold_tetromino->w;
    rect.h = hold_tetromino->h;
    SDL_BlitSurface(hold_tetromino, nullptr, surface, &rect);
    SDL_FreeSurface(hold_tetromino);
}

void TetrisMode::handle_event(SDL_Event& ev)
{
    if (ev.type == SDL_KEYDOWN) {
        if (ev.key.keysym.sym == SDLK_a) move_left();
        if (ev.key.keysym.sym == SDLK_d) move_right();
        if (ev.key.keysym.sym == SDLK_w) hard_drop();
        if (ev.key.keysym.sym == SDLK_s) soft_drop();
        if (ev.key.keysym.sym == SDLK_q) rotate_counterclockwise();
        if (ev.key.keysym.sym == SDLK_e) rotate_clockwise();
        if (ev.key.keysym.sym == SDLK_h) tetromino_hold();
    }
}

void TetrisMode::tetromino_bag_generate()
{
    // generate back
    for (int i = 0; i < Tetromino::tetrominoes_count; i++) {
        tetromino_bag.push_back(Tetromino::tetrominoes[i]());
    }

    // randomise list
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(tetromino_bag.begin(), tetromino_bag.end(), g);
}

void TetrisMode::tetromino_new()
{
    // deque tetromino from bag
    tetromino_current = tetromino_bag.back();
    tetromino_bag.pop_back();
    
    tetromino_row = 20;
    tetromino_col = 4;

    if (tetromino_collides()) {
        // that's it game over
        SDL_Event event;
        SDL_zero(event);
        // event.type = userevent_game_over;

        // SDL_PushEvent(&event);
    }

    // generate bag if empty
    if (tetromino_bag.empty()) tetromino_bag_generate();
}

bool TetrisMode::tetromino_collides()
{
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
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

void TetrisMode::tetromino_commit()
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

uint32_t TetrisMode::tick_interval()
{
    return 1000 * SDL_pow(0.8 - ((level - 1) * 0.007), level - 1);
}

Block TetrisMode::block_at(int x, int y, bool include_tetromino)
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

Block TetrisMode::block_at(int x, int y)
{
    return block_at(x, y, true);
}

void TetrisMode::tick()
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
                for (int copy_col = 0; copy_col < MATRIX_COLS; copy_col++) {
                    matrix[copy_row][copy_col] = matrix[copy_row + 1][copy_col];
                }
            }
        } else {
            line_check_row++;
        }
    }
}

void TetrisMode::move_left()
{
    tetromino_col--;
    if (tetromino_collides()) tetromino_col++;
}

void TetrisMode::move_right()
{
    tetromino_col++;
    if (tetromino_collides()) tetromino_col--;
}

void TetrisMode::soft_drop()
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

void TetrisMode::hard_drop()
{
    while (!tetromino_collides()) {
        tetromino_row--;
    }

    tetromino_row++;
    if (tetromino_row >= MATRIX_ROWS) tetromino_row = 0;

    tetromino_commit();
    tetromino_new();
}

void TetrisMode::rotate_clockwise()
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

void TetrisMode::rotate_counterclockwise()
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

void TetrisMode::tetromino_hold()
{
    // copy the tetromino over and issue a new one
    // todo: cannot hold again until next tetromino

    tetromino_held = tetromino_current;
    tetromino_new();
}

SDL_Surface* TetrisMode::render_tetromino(Tetromino tetromino)
{
auto surface = SDL_CreateRGBSurface(0,
                                        BLOCK_SIZE * 4, BLOCK_SIZE * 4,
                                        24, 0, 0, 0, 0);
    color white = { (char)0x80, (char)0x80, (char)0x80 };    

    SDL_Rect rect = { 0, 0, BLOCK_SIZE, BLOCK_SIZE };

    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            SDL_LockSurface(surface);

            color* pixels = (color*)surface->pixels;
            int base_x = x * BLOCK_SIZE;
            int base_y = y * BLOCK_SIZE;

            for (int x = 0; x < rect.w; x++) {
                pixels[(base_y) * surface->w + base_x + x] = white;
                pixels[(base_y + rect.h - 1) * surface->w + base_x + x] = white;
            }
            for (int y = 0; y < rect.h; y++) {
                pixels[(base_y + y) * surface->w + base_x] = white;
                pixels[(base_y + y) * surface->w + base_x + rect.w - 1] = white;
            }

            SDL_UnlockSurface(surface);

            rect.x += rect.w;
        }

        rect.y += rect.h;
        rect.x = 0;
    }

    rect.x = 0;
    rect.y = 0;
    for (int row = 3; row >= 0; row--) {
        for (int col = 0; col < 4; col++) {
            Block block = tetromino.blocks[row][col];
            if (block.nil) continue;

            auto color = SDL_MapRGB(surface->format, block.red, block.green, block.blue);
            SDL_FillRect(surface, &rect, color);
            
            rect.x += rect.w;
        }

        rect.x = 0;
        rect.y += rect.h;
    }

    return surface;
}

SDL_Surface* TetrisMode::render_matrix()
{
    auto surface = SDL_CreateRGBSurface(0,
                                        BLOCK_SIZE * TetrisMode::MATRIX_COLS,
                                        BLOCK_SIZE * TetrisMode::MATRIX_VISIBLE_ROWS,
                                        32, 0, 0, 0, 0);

    SDL_Rect rect = { 0, 0, BLOCK_SIZE, BLOCK_SIZE };
    for (int row = MATRIX_VISIBLE_ROWS; row >= 0; row--) {
        for (int col = 0; col < MATRIX_COLS; col++) {
            Block block = block_at(col, row);
            auto color = SDL_MapRGB(surface->format, block.red, block.green, block.blue);
            SDL_FillRect(surface, &rect, color);

            rect.x += rect.w;
        }
        
        rect.x = 0;
        rect.y += rect.h;
    }

    return surface;
}

Tetromino TetrisMode::get_next_tetromino()
{
    // generate bag if empty
    if (tetromino_bag.empty()) tetromino_bag_generate();
    return tetromino_bag[tetromino_bag.size() - 1];
}

Tetromino TetrisMode::get_held_tetromino()
{
    return tetromino_held;
}
