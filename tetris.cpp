
#include "tetris.h"

#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <exception>
#include <system_error>
#include <random>

#include <SDL.h>
#include <SDL_ttf.h>

#include "title.h"

TetrisMode::TetrisMode()
{
    using namespace std::string_literals;

    level = 1;
    state = State::PLAYING;

    srand(time(nullptr));
    tetromino_bag_generate();
    tetromino_new();

    background = SDL_LoadBMP("background.bmp");
    if (background == nullptr) {
        throw std::runtime_error("Failed to load title image: "s + SDL_GetError());
    }

    tick_timer_id = SDL_AddTimer(1000, [](uint32_t interval, void* userdata) -> uint32_t {
        TetrisMode* tetris_mode = (TetrisMode*)userdata;
        tetris_mode->tick();
        return tetris_mode->state == State::GAME_OVER ? 50 : 1000;
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
    SDL_RemoveTimer(tick_timer_id);
    SDL_FreeSurface(background);
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

    SDL_BlitScaled(background, nullptr, surface, nullptr);


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

    // score
    char score_string[64];
    sprintf(score_string, "SCORE: %8d", score);
    SDL_Surface* score_text = TTF_RenderUTF8_Solid(font, score_string, color);
    rect.w = score_text->w;
    rect.h = score_text->h;
    rect.x = 60 * 8;
    rect.y = 16 * 5;
    SDL_BlitSurface(score_text, nullptr, surface, &rect);
    SDL_FreeSurface(score_text);

    // score
    char level_string[64];
    sprintf(level_string, "LEVEL: %8d", level);
    SDL_Surface* level_text = TTF_RenderUTF8_Solid(font, level_string, color);
    rect.w = level_text->w;
    rect.h = level_text->h;
    rect.y += 16;
    SDL_BlitSurface(level_text, nullptr, surface, &rect);
    SDL_FreeSurface(level_text);

    // score
    char lines_string[64];
    sprintf(lines_string, "LINES: %8d", lines);
    SDL_Surface* lines_text = TTF_RenderUTF8_Solid(font, lines_string, color);
    rect.w = lines_text->w;
    rect.h = lines_text->h;
    rect.y += 16;
    SDL_BlitSurface(lines_text, nullptr, surface, &rect);
    SDL_FreeSurface(lines_text);
    
    // game over
    if (state == State::GAME_OVER && gameover_row > 20) {
        SDL_Surface* gameover_text = TTF_RenderUTF8_Solid(font, "GAME  OVER", color);
        rect.w = gameover_text->w;
        rect.h = gameover_text->h;
        rect.x = GameSystem::SCREEN_WIDTH / 2 - rect.w / 2;
        rect.y = 16 * 8;
        SDL_BlitSurface(gameover_text, nullptr, surface, &rect);
        SDL_FreeSurface(gameover_text);

        gameover_text = TTF_RenderUTF8_Solid(font, " Press [ENTER] to ", color);
        rect.w = gameover_text->w;
        rect.h = gameover_text->h;
        rect.x = GameSystem::SCREEN_WIDTH / 2 - rect.w / 2;
        rect.y = 16 * 16;
        SDL_BlitSurface(gameover_text, nullptr, surface, &rect);
        SDL_FreeSurface(gameover_text);

        gameover_text = TTF_RenderUTF8_Solid(font, "    start over.   ", color);
        rect.w = gameover_text->w;
        rect.h = gameover_text->h;
        rect.x = GameSystem::SCREEN_WIDTH / 2 - rect.w / 2;
        rect.y = 17 * 16;
        SDL_BlitSurface(gameover_text, nullptr, surface, &rect);
        SDL_FreeSurface(gameover_text);
    }
}

void TetrisMode::handle_event(SDL_Event& ev)
{
    if (ev.type == SDL_KEYDOWN) {
        if (state == State::PLAYING) {
            if (ev.key.keysym.sym == SDLK_LEFT) move_left();
            if (ev.key.keysym.sym == SDLK_RIGHT) move_right();
            if (ev.key.keysym.sym == SDLK_SPACE) hard_drop();
            if (ev.key.keysym.sym == SDLK_DOWN) soft_drop();
            if (ev.key.keysym.sym == SDLK_UP) rotate_counterclockwise();
            if (ev.key.keysym.sym == SDLK_END) rotate_clockwise();
            if (ev.key.keysym.sym == SDLK_h) tetromino_hold();
        }

        if (state == State::GAME_OVER) {
            if (ev.key.keysym.sym == SDLK_RETURN || ev.key.keysym.sym == SDLK_RETURN2) {
                game_system->set_mode(new TitleScreen());
            }
        }
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
    tetromino_new(true);
}

void TetrisMode::tetromino_new(bool deque)
{
    // deque tetromino from bag
    if (deque) {
        tetromino_current = tetromino_bag.back();
        tetromino_bag.pop_back();
    }

    tetromino_row = 20;
    tetromino_col = 4;

    if (tetromino_collides()) {
        // that's it game over
        gameover_row = 0;
        state = State::GAME_OVER;

        tetromino_current.nil = true;
    }

    // generate bag if empty
    if (tetromino_bag.empty()) tetromino_bag_generate();

    tetromino_source = TetrominoSource::BAG;
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
    if (include_tetromino && !tetromino_current.nil) {
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
    if (state == State::PLAYING) tick_game();
    if (state == State::GAME_OVER) tick_gameover();
}

void TetrisMode::tick_game()
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
    int line_clear = 0;
    int line_check_row = 0;
    while (line_check_row < MATRIX_ROWS) {
        int block_count = 0;
        
        for (int col = 0; col < MATRIX_COLS; col++) {
            if (!block_at(col, line_check_row, false).nil) block_count++;
        }

        if (block_count == MATRIX_COLS) {
            line_clear++;
            SDL_Log("Line clear: %d so far.", line_clear);
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

    // calculate line clear score
    if (line_clear > 0) {
        float multiplier = 100;
        multiplier += 200 * (line_clear - 1);
        if (line_clear == 4) multiplier += 100;
        score += multiplier * level;
        SDL_Log("Line clear: %d lines : x%f multiplier", line_clear, multiplier);

        lines += line_clear;
    }
}

void TetrisMode::tick_gameover()
{
    if (gameover_row > 20) return;

    Block block;
    block.red = 0x20;
    block.green = 0x20;
    block.blue = 0x20;
    block.nil = true;

    for (int x = 0; x < MATRIX_COLS; x++) {
        matrix[gameover_row][x] = block;
    }
    gameover_row++;
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
    score += 1;
    SDL_Log("Soft drop: 1 row");
    
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
    int row_start = tetromino_row;
    int row_diff;

    while (!tetromino_collides()) {
        tetromino_row--;
    }

    tetromino_row++;
    if (tetromino_row >= MATRIX_ROWS) tetromino_row = 0;

    row_diff = abs(tetromino_row - row_start);
    score += 2 * SDL_abs(row_start - tetromino_row);

    tetromino_commit();
    tetromino_new();

    SDL_Log("Hard drop: %d rows", row_diff);
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

    if (tetromino_source == TetrominoSource::HOLD) return;
    if (tetromino_current.nil) return;
    
    Tetromino held = tetromino_held;
    tetromino_held = tetromino_current;
    if (held.nil) {
        tetromino_new();
    } else {
        tetromino_current = held;
        tetromino_new(false);
    }

    tetromino_source = TetrominoSource::HOLD;
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
