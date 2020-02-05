#pragma once

#include <string>
#include <vector>

#include <SDL.h>
#include <SDL_ttf.h>

#include "mode.h"

// the game system -- or engine if you prefer
class GameSystem {
public:
    // screen size
    static const unsigned int SCREEN_WIDTH = 640;
    static const unsigned int SCREEN_HEIGHT = 360;

private:
    // currently focused mode
    GameMode* mode = nullptr;

    // awaiting quit
    int exit_code = 0;
public:
    bool quit_triggered = false;
    bool quit_now = false;

private:
    // game window
    SDL_Window* window = nullptr;
    TTF_Font* font = nullptr;

    // command-line arguments
    std::vector<std::string> cmd_args;

    // common constructor
    void common_ctor();

    // event loop
    void event_loop();

    // draw callback
    bool draw_progress = false;         // drawing in progress
    bool draw_block = false;            // prevent drawing
    uint32_t draw_interval = 1000 / 60; // timer interval (from target FPS)
    SDL_TimerID draw_timer_id = 0;

public:
    GameSystem();
    GameSystem(std::vector<std::string>&);
    ~GameSystem();

    // run the game system
    void run();

    // get final exit code
    int get_exit_code();

    GameMode* get_mode();
    void set_mode(GameMode*);

    SDL_Window* get_window();
    TTF_Font* get_font();

    uint32_t draw_callback(uint32_t, void*); 
    void set_draw_interval(uint32_t);
    void set_draw_target_fps(double);
};

// global instance for all
extern GameSystem* game_system;