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

	void drawFilledCircle(int x0, int y0, int r, Color c) const;
	void drawFilledRect(int x, int y, int hw, int hh, Color c) const;
	void drawText(const char* text, int x, int y, SDL_Color color, TTF_Font* font) const;

private:
    Renderer() = default;
};