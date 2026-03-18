#pragma once

#include "Input.h"
#include "Renderer.h"
#include "World.h"

#include <SDL.h>
#include <SDL_ttf.h>
#include <memory>


constexpr int SCREEN_WIDTH = 800;
constexpr int SCREEN_HEIGHT = 600;


class PhysicsEngine {
private:
	PhysicsEngine() = default;

	bool createWindow(SDL_Window** window);
	bool createRenderer(SDL_Renderer** renderer);
	bool createFont(TTF_Font** font);

	void render();

	bool running = true; 

	SDL_Window* sdlWindow = nullptr;
	SDL_Event sdlEvent;

	TTF_Font* font = nullptr;
	SDL_Color textColor = Color::WHITE.toSDLColor();

public:
	static PhysicsEngine* getInstance() {
		static PhysicsEngine INSTANCE;
		return &INSTANCE;
	}

	int run(int argc, char** argv);
	void quit();

	Renderer* renderer = Renderer::getInstance();
	Input* input = Input::getInstance();
	std::unique_ptr<World> world;

	float currentRadius = 20.0f;
	ShapeType shape = ShapeType::CIRCLE;

	void requestQuit() {
		running = false;
	}
};

enum Action {
	PRESS = 0,
	RELEASE = 1,
	REPEAT = 2
};

enum MouseButton {
	LEFT = 1,
	MIDDLE = 2,
	RIGHT = 3,
	BUTTON_4 = 4,
	BUTTON_5 = 5
};