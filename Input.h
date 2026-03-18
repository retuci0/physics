#pragma once

#include "Vec2.h"
#include "RigidBody.h"
#include <SDL.h>


class Input {
public:
	static Input* getInstance() {
		static Input INSTANCE;
		return &INSTANCE;
	}

	RigidBody* selected = nullptr;

    Vec2f lastMousePos;
    Uint32 lastMouseTime;
    bool dragging = false;

    void tick(const SDL_Event& event);

	void onKey(int key, int action);
	void onClick(int button, int action, int mx, int my);
	void onMouseMove(int mx, int my);
	void onMouseWheel(int delta);

	void selectBody(const Vec2f& mousePos, RigidBody* body);
	void unselectBody();
};