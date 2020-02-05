
#include "title.h"

#include <exception>
#include <system_error>

#include "tetris.h"

using namespace std::string_literals;

TitleScreen::TitleScreen()
{
    title_image = SDL_LoadBMP("tetris.bmp");
    if (title_image == nullptr) {
        throw std::runtime_error("Failed to load title image: "s + SDL_GetError());
    }
}

TitleScreen::~TitleScreen()
{
    SDL_FreeSurface(title_image);
}

void TitleScreen::initialise()
{
    SDL_LogDebug(SDL_LOG_CATEGORY_SYSTEM, "[TitleScreen::initialise] Stub.");
}

void TitleScreen::deinitialise()
{
    SDL_LogDebug(SDL_LOG_CATEGORY_SYSTEM, "[TitleScreen::deinitialise] Stub.");
}

void TitleScreen::draw(SDL_Surface* surface)
{
    auto font = game_system->get_font();
    SDL_Color color = { 255, 255, 255, 255 };

    SDL_BlitScaled(title_image, nullptr, surface, nullptr);
    auto faded_black = SDL_MapRGBA(surface->format, 0, 0, 0, 128);
    SDL_Rect rect = { 16, 180 + 32, 608, 180 - 32 - 16 };
    SDL_FillRect(surface, &rect, faded_black);

    SDL_Surface* press_start = TTF_RenderUTF8_Solid(font, "Press [ENTER] to play.", color);
    rect.x = GameSystem::SCREEN_WIDTH / 2 - press_start->w / 2;
    rect.y = 15 * 16;
    rect.w = press_start->w;
    rect.h = press_start->h;
    SDL_BlitSurface(press_start, nullptr, surface, &rect);
    SDL_FreeSurface(press_start);

    SDL_Surface* top_score = TTF_RenderUTF8_Solid(font, "TOP SCORE: 00000000", color);
    rect.x = GameSystem::SCREEN_WIDTH / 2 - top_score->w / 2;
    rect.y += 16;
    rect.w = top_score->w;
    rect.h = top_score->h;
    SDL_BlitSurface(top_score, nullptr, surface, &rect);
    SDL_FreeSurface(top_score);

    SDL_Surface* copyright = TTF_RenderUTF8_Solid(font, "© 2020  Raresh Nistor / Рареш Нистор", color);
    rect.x = GameSystem::SCREEN_WIDTH / 2 - press_start->w / 2;
    rect.y += 16 * 2;
    rect.w = copyright->w;
    rect.h = copyright->h;
    SDL_BlitSurface(copyright, nullptr, surface, &rect);
    SDL_FreeSurface(copyright);

    copyright = TTF_RenderUTF8_Solid(font, " Tetris™ is registered by The Tetris Company", color);
    rect.x = GameSystem::SCREEN_WIDTH / 2 - copyright->w / 2;
    rect.y += 16;
    rect.w = copyright->w;
    rect.h = copyright->h;
    SDL_BlitSurface(copyright, nullptr, surface, &rect);
    SDL_FreeSurface(copyright);
}

void TitleScreen::handle_event(SDL_Event& event)
{
    if (event.type == SDL_QUIT) {
        game_system->quit_now = true;
    }

    if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_RETURN2) {
            game_system->set_mode(new TetrisMode());
        }
    }
}
