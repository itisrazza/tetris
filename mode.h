#pragma once

#include <SDL.h>

class GameMode;

#include "system.h"

class GameMode {
public:
    virtual ~GameMode() = 0;

    // run to set up game mode
    virtual void initialise() = 0;

    // run when game mode is about to be changed
    virtual void deinitialise() = 0;

    // run when game is rendering video
    virtual void draw(SDL_Surface*) = 0;

    // handle event polled from queue
    virtual void handle_event(SDL_Event&) = 0;
};
