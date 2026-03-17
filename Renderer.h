#pragma once

#include "Color.h"
#include <SDL.h>
#include <SDL_ttf.h>

class Renderer {
public:
    SDL_Renderer* renderer = nullptr;

	static Renderer* getInstance() {
		static Renderer INSTANCE;
		return &INSTANCE;
	}

    void drawFilledCircle(int x0, int y0, int r, Color c) const {
        SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
        for (int y = -r; y <= r; y++) {
            for (int x = -r; x <= r; x++) {
                if (x * x + y * y <= r * r) {
                    SDL_RenderDrawPoint(renderer, x0 + x, y0 + y);
                }
            }
        }
    }

    void drawText(const char* text, int x, int y, SDL_Color color, TTF_Font* font) {
        if (!font || !text) return;

        SDL_Surface* surface = TTF_RenderText_Blended(font, text, color);
        if (!surface) return;

        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_Rect dest = { x, y, surface->w, surface->h };

        SDL_RenderCopy(renderer, texture, NULL, &dest);

        SDL_DestroyTexture(texture);
        SDL_FreeSurface(surface);
    }

private:
    Renderer() = default;
};