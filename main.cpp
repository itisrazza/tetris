
#include <SDL.h>
#include "tetris.h"

SDL_Window* window;
bool draw_block = false;
const unsigned int WINDOW_WIDTH = 640;
const unsigned int WINDOW_HEIGHT = 320;

TetrisGame tetris;
const int BLOCK_SIZE = 12;

int status_code = 0;

bool init()
{
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
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
    return true;
}

void clean()
{

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
        }
    }

    return true;
}

// update the tetris matrix
uint32_t update_matrix(uint32_t interval, void *param)
{
    // lock the drawing
    draw_block = true;
    tetris.tick();
    draw_block = false;

    // update to make levels possible later
    // return tetris.level_interval();
    return 1000;
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

    SDL_Surface* surface = SDL_CreateRGBSurface(0, WINDOW_WIDTH, WINDOW_HEIGHT, 24, 0, 0, 0, 0);
    SDL_Surface* window_surface = SDL_GetWindowSurface(window);

    int window_width, window_height;
    SDL_GetWindowSize(window, &window_width, &window_height);
    
    SDL_Surface* matrix_surface = render_matrix();
    SDL_Rect rect = { WINDOW_WIDTH / 2 - matrix_surface->w / 2,
                      WINDOW_HEIGHT / 2 - matrix_surface->h / 2,
                      matrix_surface->w, matrix_surface->h };

    rect.x -= 1;
    rect.y -= 1;
    rect.w += 2;
    rect.h += 2;
    SDL_FillRect(surface, &rect, SDL_MapRGB(surface->format, 255, 255, 255));

    rect.x += 1;
    rect.y += 1;
    rect.w -= 2;
    rect.h -= 2;
    SDL_BlitSurface(matrix_surface, nullptr, surface, &rect);
    
    SDL_BlitScaled(surface, nullptr, window_surface, nullptr);

    SDL_UpdateWindowSurface(window);
    SDL_FreeSurface(matrix_surface);
    SDL_FreeSurface(surface);
    SDL_FreeSurface(window_surface);

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
