#pragma once

#include <SDL.h>

#include "mode.h"

class TitleScreen : public GameMode {
    SDL_Surface* title_image;

public:
    TitleScreen();
    ~TitleScreen();

    void initialise();
    void deinitialise();

    void draw(SDL_Surface*);
    void handle_event(SDL_Event&);
};
