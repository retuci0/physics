#pragma once

#include "World.h"
#include "Renderer.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include <memory>


class PhysicsEngine {
private:
	PhysicsEngine() = default;

	bool createWindow(SDL_Window** window);
	bool createRenderer(SDL_Renderer** renderer);
	bool createFont(TTF_Font** font);

	void onKey(int key, int action);
	void onClick(int button, int action, int mx, int my);
	void onMouseMove(int mx, int my);
	void onMouseWheel(int delta);

	void render();

	bool running = true; 

	float currentRadius = 20.0f;

	Vec2f lastMousePos;
	Uint32 lastMouseTime;
	bool dragging = false;

	SDL_Window* sdlWindow = nullptr;
	SDL_Event sdlEvent;

	RigidBody* selected = nullptr;

	Renderer* renderer = Renderer::getInstance();
	std::unique_ptr<World> world;

	TTF_Font* font = nullptr;
	SDL_Color textColor = Color::WHITE.toSDLColor();

public:
	int run(int argc, char** argv);
	void quit();

	static PhysicsEngine* getInstance() {
		static PhysicsEngine INSTANCE;
		return &INSTANCE;
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