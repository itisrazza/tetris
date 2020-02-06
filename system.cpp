
#include "system.h"

#include <exception>
#include <system_error>

#include "title.h"
#include "tetris.h"

using namespace std::string_literals;

GameSystem* game_system = nullptr;

//
// main -> GameSystem bootstrap
//

int main(int argc, char** argv) {
    SDL_Log("Starting Tetris.");

    // aggregate argv into a vector
    std::vector<std::string> args;
    for (int i = 0; i < argc; i++) {
        args.push_back(argv[i]);
    }

    // write errors to console in early startup
    try {
        game_system = new GameSystem(args);
        game_system->run();
        int exit_code = game_system->get_exit_code();
        delete game_system;
        return exit_code;
    } catch (std::exception& ex) {
        SDL_LogCritical(SDL_LOG_CATEGORY_ERROR, "C++ exception: %s", ex.what());
        return 1;
    }
}

void GameSystem::common_ctor()
{
    // init SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        throw std::runtime_error("Failed to load SDL: "s + std::string(SDL_GetError()));
    }

    // init TTF
    if (TTF_Init() < 0) {
        throw std::runtime_error("Failed to init TTF: "s + SDL_GetError());
    }

    // create window
    window = SDL_CreateWindow("Тетрис Рареша | Raresh's Tetris",
                              SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    if (window == nullptr) {
        throw std::runtime_error("Failed to create window: "s + SDL_GetError());
    }

    // load font
    font = TTF_OpenFont("font.ttf", 16);
    if (font == nullptr) {
        throw std::runtime_error("Failed to load font: "s + SDL_GetError());
    }

    // start with a title screen
    set_mode(new TitleScreen());
}

GameSystem::GameSystem()
{
    this->common_ctor();
}

GameSystem::GameSystem(std::vector<std::string>& args)
{
    cmd_args = args;
    this->common_ctor();
}

GameSystem::~GameSystem()
{
	SDL_Log("Descructing system.");

	// deinit mode
	mode->deinitialise();
	delete mode;

	// free SDL resources
    SDL_DestroyWindow(window);
    TTF_CloseFont(font);

	// close libraries
	TTF_Quit();
	SDL_Quit();
}

//
// event handling
//

void GameSystem::event_loop()
{
    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
        if (ev.type == SDL_QUIT) {
            if (quit_triggered) {
                // force quit
                SDL_Log("Quitting now...");
                quit_now = true;
            } else {
                // confirm first
                SDL_Log("Quit triggered. Awaiting confirmation.");
                quit_triggered = true;
            }
        }
        
        if (ev.type == SDL_KEYDOWN) {
            // global key events

            if (ev.key.keysym.sym == SDLK_ESCAPE) {
                quit_now = true;
            }
            if (ev.key.keysym.sym == SDLK_F11) {
                int full_screen = SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN_DESKTOP;
                SDL_SetWindowFullscreen(window, full_screen ? 0 : SDL_WINDOW_FULLSCREEN_DESKTOP);
            }
        }

        // pass the event down to current mode
        if (mode != nullptr) mode->handle_event(ev);
    }
}

void GameSystem::run()
{
    // register draw timer
    draw_timer_id = SDL_AddTimer(draw_interval, [](uint32_t interval, void* userdata) -> uint32_t {
        GameSystem* game_system = (GameSystem*)userdata;
        game_system->draw_callback(interval, userdata);
		return interval;
    }, this);

    // wait for quitting and dequeue events
    while (!quit_now) event_loop();
}

//
// draw
//

uint32_t GameSystem::draw_callback(uint32_t interval, void* userdata)
{
    if (draw_block) return draw_interval;
    if (draw_progress) return draw_interval;
    if (mode == nullptr) return draw_interval;

    draw_progress = true;
    
    // create screen surface
    SDL_Surface* surface = SDL_CreateRGBSurface(0,
                                                SCREEN_WIDTH, SCREEN_HEIGHT,
                                                24, 0, 0, 0, 0);
    if (surface == nullptr) {
        throw std::runtime_error("Failed to create surface: "s + SDL_GetError());
    }
    
    // pass it down to be edited by focused mode
	if (mode != nullptr) {
		mode->draw(surface);
	}

    // blit it to window and go
    SDL_Surface* window_surface = SDL_GetWindowSurface(window);
    SDL_BlitScaled(surface, nullptr, window_surface, nullptr);
    SDL_UpdateWindowSurface(window);
    SDL_FreeSurface(surface);

    draw_progress = false;

    return draw_interval;
}

void GameSystem::set_draw_interval(uint32_t interval)
{
    draw_interval = interval;
}

void GameSystem::set_draw_target_fps(double target_fps)
{
    draw_interval = (int)(1000 / target_fps);
}

//
// mode getter/setter
//

GameMode* GameSystem::get_mode()
{
    return mode;
}

void GameSystem::set_mode(GameMode* mode)
{
    // cleanup and deallocate old mode
    if (this->mode != nullptr) {
        this->mode->deinitialise();
        delete this->mode;
    }

    // initialise and setup new one
    this->mode = mode;
    this->mode->initialise();
}

//
// window and font getter
//

SDL_Window* GameSystem::get_window()
{
    return this->window;
}

TTF_Font* GameSystem::get_font()
{
    return this->font;
}

//
// exit code getter
//

int GameSystem::get_exit_code()
{
    return this->exit_code;
}
