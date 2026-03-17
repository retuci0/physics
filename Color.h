#pragma once

#include <SDL.h>


class Color {
public:
	Uint32 r, g, b, a;
	Color(Uint32 r, Uint32 g, Uint32 b, Uint32 a) : r(r), g(g), b(b), a(a) {}
	Color(int rgba) {
		a = (rgba >> 24) & 0xFF;
		r = (rgba >> 16) & 0xFF;
		g = (rgba >>  8) & 0xFF;
		b = (rgba >>  0) & 0xFF;
	}

	static Color WHITE;
	static Color BLACK;
	static Color RED;
	static Color GREEN;
	static Color BLUE;
	static Color SILVER;

	Color operator+(const Color& c) {
		return Color(
			r + c.r,
			g + c.g,
			b + c.b,
			a + c.a
		);
	}

	Color operator-(const Color& c) {
		return Color(
			r - c.r,
			g - c.g,
			b - c.b,
			a - c.a
		);
	}

	int getRGB() const {
		return (a << 24) 
			 | (r << 16) 
			 | (g <<  8) 
			 | (b <<  0);
	}

	SDL_Color toSDLColor() {
		return SDL_Color{
			static_cast<Uint8>(r),
			static_cast<Uint8>(g),
			static_cast<Uint8>(b),
			static_cast<Uint8>(a)
		};
	}
};