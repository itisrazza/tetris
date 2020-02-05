
#include <SDL.h>
#include <SDL_ttf.h>

#include "tetris.h"
#include "font.h"

SDL_Window* window;
bool draw_block = false;
const unsigned int WINDOW_WIDTH = 640;
const unsigned int WINDOW_HEIGHT = 360;

TTF_Font* font;

TetrisGame tetris;
const int BLOCK_SIZE = 16;

int status_code = 0;
float shake = 0.0;

int draw_matrix_timer_id = 0;
int update_matrix_timer_id = 0;

bool playing = true;

struct Config {
    unsigned int window_scale;
    bool fullscreen_integer;
};

Config config;

bool init()
{
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        return false;
    }

    if (TTF_Init() < 0) {
        return false;
    }

    window = SDL_CreateWindow("Тетрис Рареша | Raresh's Tetris",
                              SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              WINDOW_WIDTH,
                              WINDOW_HEIGHT,
                              0);
    if (window == nullptr) {
        return false;
    }

    return true;
}

bool load_media()
{
    font = TTF_OpenFont("font.ttf", 16);
    if (font == nullptr) {
        SDL_LogCritical(SDL_LOG_CATEGORY_ERROR, "Failed to open font: %s", TTF_GetError());
        return false;
    }

    return true;
}

void clean()
{
    SDL_DestroyWindow(window);
    TTF_CloseFont(font);
}

bool event_loop()
{
    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
        if (ev.type == SDL_QUIT) return false;
        if (ev.type == SDL_KEYDOWN) {
            if (ev.key.keysym.sym == SDLK_ESCAPE) return false;
            if (ev.key.keysym.sym == SDLK_F11) {
                int full_screen = SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN_DESKTOP;
                SDL_SetWindowFullscreen(window, full_screen ? 0 : SDL_WINDOW_FULLSCREEN_DESKTOP);
            }
            if (ev.key.keysym.sym == SDLK_a) tetris.move_left();
            if (ev.key.keysym.sym == SDLK_d) tetris.move_right();
            if (ev.key.keysym.sym == SDLK_w) tetris.hard_drop();
            if (ev.key.keysym.sym == SDLK_s) tetris.soft_drop();
            if (ev.key.keysym.sym == SDLK_q) tetris.rotate_counterclockwise();
            if (ev.key.keysym.sym == SDLK_e) tetris.rotate_clockwise();
            if (ev.key.keysym.sym == SDLK_h) tetris.tetromino_hold();
        }
        if (ev.type == tetris.userevent_game_over) {
            shake = 8;
            playing = false;
        }
    }

    return true;
}

// update the tetris matrix
uint32_t update_matrix(uint32_t interval, void *param)
{
    if (!playing) return 0;

    // lock the drawing
    draw_block = true;
    tetris.tick();
    draw_block = false;

    // update to make levels possible later
    // return tetris.level_interval();
    return 1000;
}

struct color { char r; char g; char b; };

// renders a tetromino
SDL_Surface* render_tetromino(Tetromino tetromino)
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

// renders the matrix -- SDL_Surface* must be freed after use
SDL_Surface* render_matrix()
{
    auto surface = SDL_CreateRGBSurface(0,
                                        BLOCK_SIZE * TetrisGame::MATRIX_COLS,
                                        BLOCK_SIZE * TetrisGame::MATRIX_VISIBLE_ROWS,
                                        32, 0, 0, 0, 0);

    SDL_Rect rect = { 0, 0, BLOCK_SIZE, BLOCK_SIZE };
    for (int row = TetrisGame::MATRIX_VISIBLE_ROWS; row >= 0; row--) {
        for (int col = 0; col < TetrisGame::MATRIX_COLS; col++) {
            Block block = tetris.block_at(col, row);
            auto color = SDL_MapRGB(surface->format, block.red, block.green, block.blue);
            SDL_FillRect(surface, &rect, color);

            rect.x += rect.w;
        }
        
        rect.x = 0;
        rect.y += rect.h;
    }

    return surface;
}

// draw the matrix on-screen
uint32_t draw_matrix(uint32_t interval, void *param)
{
    // don't draw if matrix is changing
    if (draw_block) return interval;

    // create a surface for screen things
    SDL_Surface* surface = SDL_CreateRGBSurface(0, WINDOW_WIDTH, WINDOW_HEIGHT, 24, 0, 0, 0, 0);
    
    // get actual window size
    int window_width, window_height;
    SDL_GetWindowSize(window, &window_width, &window_height);
    
    // draw the matrix on-screen
    SDL_Surface* matrix_surface = render_matrix();
    SDL_Rect rect = { WINDOW_WIDTH / 2 - matrix_surface->w / 2,
                      WINDOW_HEIGHT / 2 - matrix_surface->h / 2,
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

    if (shake > 0) {
        rect.x += shake * ((double)rand() - RAND_MAX / 2.0) / RAND_MAX;
        rect.y += shake * ((double)rand() - RAND_MAX / 2.0) / RAND_MAX;
        shake -= 0.1;
    }

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
    auto next_tetromino = render_tetromino(tetris.get_next_tetromino());
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
    auto hold_tetromino = render_tetromino(tetris.get_held_tetromino());
    rect.x += rect.w - hold_tetromino->w;
    rect.y += 16;
    rect.w = hold_tetromino->w;
    rect.h = hold_tetromino->h;
    SDL_BlitSurface(hold_tetromino, nullptr, surface, &rect);
    SDL_FreeSurface(hold_tetromino);

    // apply the base surface to window
    SDL_Surface* window_surface = SDL_GetWindowSurface(window);
    SDL_BlitScaled(surface, nullptr, window_surface, nullptr);
    SDL_UpdateWindowSurface(window);
    
    // free the base surface.. we don't need it anymore
    SDL_FreeSurface(surface);

    return interval;
}

int main(int argc, char** argv)
{
    if (!init()) return 1;
    if (!load_media()) return 1;

    SDL_AddTimer(1000 / 60, draw_matrix, nullptr);
    SDL_AddTimer(1000, update_matrix, nullptr);

    while (event_loop()) ;
    clean();
    return status_code;
}
