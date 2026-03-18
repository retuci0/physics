#include "Input.h"
#include "Physics.h"

#include <algorithm>
#include <memory>


void Input::tick(const SDL_Event& event) {
    PhysicsEngine* pe = PhysicsEngine::getInstance();

    // cerrar ventana
    if (event.type == SDL_QUIT) {
        pe->requestQuit();
    }

    // gestión de teclas
    else if (event.type == SDL_KEYDOWN) {
        if (event.key.repeat) {
            onKey(event.key.keysym.sym, Action::REPEAT);
        } else {
            onKey(event.key.keysym.sym, Action::PRESS);
        }
    } else if (event.type == SDL_KEYUP) {
        onKey(event.key.keysym.sym, Action::RELEASE);
    }

    // gestión del ratón
    else if (event.type == SDL_MOUSEBUTTONDOWN) {
        onClick(event.button.button, Action::PRESS, event.button.x, event.button.y);
    } else if (event.type == SDL_MOUSEBUTTONUP) {
        onClick(event.button.button, Action::RELEASE, event.button.x, event.button.y);
    } else if (event.type == SDL_MOUSEWHEEL) {
        onMouseWheel(event.wheel.y);
    }
}

// teclado
void Input::onKey(int key, int action) {
    if (action == Action::PRESS) {
        keys[key] = true;
    } else if (action == Action::RELEASE) {
        keys[key] = false;
    }

    if (action == Action::PRESS) {
        if (key == SDLK_ESCAPE) {
             PhysicsEngine::getInstance()->requestQuit();
        } else {
            PhysicsEngine* pe = PhysicsEngine::getInstance();
            switch (key) {
                case SDLK_1:
                    pe->shape = ShapeType::CIRCLE;
                    break;
                case SDLK_2:
                    pe->shape = ShapeType::RECT;
                    break;
            }
        }
    }
}

// clic del ratón
void Input::onClick(int button, int action, int mx, int my) {
    PhysicsEngine* pe = PhysicsEngine::getInstance();

    // crear cuerpos
    if (button == MouseButton::MIDDLE && action == Action::PRESS) {
        switch (pe->shape) {
            case ShapeType::CIRCLE:
                pe->world->addBody(std::make_unique<RigidBody>(Vec2f(mx, my), 1.0f, std::make_unique<CircleShape>(pe->currentRadius)));
                break;
            case ShapeType::RECT:
                pe->world->addBody(std::make_unique<RigidBody>(Vec2f(mx, my), 1.0f, std::make_unique<RectangleShape>(pe->currentRadius, pe->currentRadius)));
                break;
        }
    }

    // eliminar cuerpos
    if (button == MouseButton::RIGHT && action == Action::PRESS) {
        for (auto& body : pe->world->getBodies()) {
            Vec2f mousePos(mx, my);
            if (body->contains(mousePos)) {
                pe->world->removeBody(body.get());
                break;
            }
        }
    }

    // agarrar cuerpo
    if (button == MouseButton::LEFT && action == Action::PRESS) {
        for (auto& body : pe->world->getBodies()) {
            Vec2f mousePos(mx, my);
            if (body->contains(mousePos)) {
                selectBody(mousePos, body.get());
                break;
            }
        }
    }

    // soltar cuerpo
    else if (button == MouseButton::LEFT && action == Action::RELEASE) {
        unselectBody();
    }
}

// mover ratón
void Input::onMouseMove(int mx, int my) {
    if (selected
        && (mx > SCREEN_WIDTH
            || mx < 0
            || my > SCREEN_HEIGHT
            || my < 0)) {
        unselectBody();
        return;
    }

    if (selected && dragging) {
        Uint32 currentTime = SDL_GetTicks();
        float dt = (currentTime - lastMouseTime) / 1000.0f;
        if (dt > 0) {
            Vec2f newPos(mx, my);
            Vec2f dr = newPos - lastMousePos;
            selected->vel = dr / dt;
            selected->pos = newPos;
            lastMousePos = newPos;
            lastMouseTime = currentTime;
        }
    }
}

// ruedita
void Input::onMouseWheel(int delta) {
    PhysicsEngine* pe = PhysicsEngine::getInstance();
    pe->currentRadius = std::clamp(pe->currentRadius + delta, 1.0f, 200.0f);
}


bool Input::isKeyDown(int key) const {
    return keys[key];
}

// agarrar un cuerpo
void Input::selectBody(const Vec2f& mousePos, RigidBody* body) {
    selected = body;
    lastMousePos = mousePos;
    lastMouseTime = SDL_GetTicks();
    dragging = true;
}

// soltar un cuerpo
void Input::unselectBody() {
    selected = nullptr;
    dragging = false;
}