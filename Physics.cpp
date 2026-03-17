#include "Physics.h"

#include "Color.h"
#include "Renderer.h"

#include <SDL.h>
#include <SDL_ttf.h>

#include <algorithm>
#include <cmath>
#include <iostream>
#include <memory>


constexpr int SCREEN_WIDTH = 800;
constexpr int SCREEN_HEIGHT = 600;

PhysicsEngine* engine = PhysicsEngine::getInstance();


int main(int argc, char** argv) {
    return engine->run(argc, argv);
}


int PhysicsEngine::run(int argc, char** argv) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "error al inicializar SDL: " << SDL_GetError() << std::endl;
        return -1;
    }

    // crear ventana y renderizador (con acel. por hw)
    if (!createWindow(&sdlWindow) 
            || !createRenderer(&renderer->renderer) 
            || !createFont(&font)) 
        return -1;

    // crear mundo
    world = std::make_unique<World>(Vec2f(0, 9.8 * 60), 0, SCREEN_WIDTH, 0, SCREEN_HEIGHT);

    // crear cuerpos de prueba
    auto shape1 = std::make_unique<CircleShape>(20.0f);
    auto ball1 = std::make_unique<RigidBody>(Vec2f(300, 100), 1.0f, std::move(shape1));
    world->addBody(std::move(ball1));

    auto shape2 = std::make_unique<CircleShape>(20.0f);
    auto ball2 = std::make_unique<RigidBody>(Vec2f(500, 100), 1.0f, std::move(shape2));
    world->addBody(std::move(ball2));

    // bucle prinipal, gestión de eventos
    Uint32 lastTime = SDL_GetTicks();
    while (running) {
        Uint32 currentTime = SDL_GetTicks();
        float dt = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;

        while (SDL_PollEvent(&sdlEvent) != 0) {
            // cerrar ventana
            if (sdlEvent.type == SDL_QUIT) {
                running = false;
            }

            // gestión de teclas
            else if (sdlEvent.type == SDL_KEYDOWN) {
                if (sdlEvent.key.repeat) {
                    onKey(sdlEvent.key.keysym.sym, Action::REPEAT);
                } else {
                    onKey(sdlEvent.key.keysym.sym, Action::PRESS);
                }
            }  else if (sdlEvent.type == SDL_KEYUP) {
                onKey(sdlEvent.key.keysym.sym, Action::RELEASE);
            }

            // gestión del ratón
            else if (sdlEvent.type == SDL_MOUSEBUTTONDOWN) {
                onClick(sdlEvent.button.button, Action::PRESS, sdlEvent.motion.x, sdlEvent.motion.y);
            } else if (sdlEvent.type == SDL_MOUSEBUTTONUP) {
                onClick(sdlEvent.button.button, Action::RELEASE, sdlEvent.motion.x, sdlEvent.motion.y);
            } else if (sdlEvent.type == SDL_MOUSEMOTION) {
                onMouseMove(sdlEvent.motion.x, sdlEvent.motion.y);
            } else if (sdlEvent.type == SDL_MOUSEWHEEL) {
                onMouseWheel(sdlEvent.wheel.y);
            }
        }
        
        world->tick(dt);
        render();
    }

    quit();
    return 0;
}

bool PhysicsEngine::createWindow(SDL_Window** window) {
    *window = SDL_CreateWindow(
        "motor de físicas",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN
    );

    if (*window == nullptr) {
        std::cerr << "error al crear la ventana: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return false;
    }

    return true;
}

bool PhysicsEngine::createRenderer(SDL_Renderer** renderer) {
    *renderer = SDL_CreateRenderer(
        sdlWindow,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    if (*renderer == nullptr) {
        std::cerr << "error al crear el renderizador: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(sdlWindow);
        SDL_Quit();
        return false;
    }

    return true;
}

bool PhysicsEngine::createFont(TTF_Font** font) {
    if (TTF_Init() == -1) {
        std::cerr << "error al inicializar SDL_ttf: " << TTF_GetError() << std::endl;
        return false;
    }

    *font = TTF_OpenFont("ubuntu.ttf", 16);
    if (!*font) {
        std::cerr << "error al cargar la fuente: " << TTF_GetError() << std::endl;
        return false;
    }

    return true;
}


/* EVENTOS */

// teclado
void PhysicsEngine::onKey(int key, int action) {
    if (key == SDLK_ESCAPE && action == Action::PRESS) {
        running = false;
    }
}

// clic del ratón
void PhysicsEngine::onClick(int button, int action, int mx, int my) {
    // crear nuevo cuerpo
    if (button == MouseButton::MIDDLE && action == Action::PRESS) {
        world->addBody(std::make_unique<RigidBody>(Vec2f(mx, my), 1.0f, std::make_unique<CircleShape>(currentRadius)));
    }

    // agarrar cuerpo
    if (button == MouseButton::LEFT && action == Action::PRESS) {
        for (auto& body : world->getBodies()) {
            if (auto circle = dynamic_cast<CircleShape*>(body->shape.get())) {
                Vec2f mousePos(mx, my);
                if ((body->pos - mousePos).sqlen() <= circle->radius * circle->radius) {
                    selected = body.get();
                    lastMousePos = mousePos;
                    lastMouseTime = SDL_GetTicks();
                    dragging = true;
                    break;
                }
            }
        }
    } 
    
    // soltar cuerpo
    else if (button == MouseButton::LEFT && action == Action::RELEASE) {
        selected = nullptr;
        dragging = false;
    }
}

// mover ratón
void PhysicsEngine::onMouseMove(int mx, int my) {
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
void PhysicsEngine::onMouseWheel(int delta) {
    currentRadius = std::clamp(currentRadius + delta, 1.0f, 200.0f);
}



void PhysicsEngine::render() {
    SDL_SetRenderDrawColor(renderer->renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(renderer->renderer);

    // renderizar cuerpos
    for (auto &body : world->getBodies()) {
        if (auto circle = dynamic_cast<CircleShape*>(body->shape.get())) {
            renderer->drawFilledCircle(
                static_cast<int>(std::round(body->pos.x)),
                static_cast<int>(std::round(body->pos.y)),
                circle->radius,
                Color::SILVER
            );
        }
    }

    // texto
    if (font) {
        renderer->drawText("clic izq.: arrastrar bola | ruedita (pulsar): crear bola | ruedita: cambiar tamaño de bola",
            10, 10, textColor, font);

        char buffer[128];
        snprintf(buffer, sizeof(buffer), "radio de bola: %.2f", currentRadius);
        renderer->drawText(buffer, 10, 40, textColor, font);

        // si hay una bola seleccionada, mostrar sus coordenadas
        if (selected) {
            char buffer[128];
            snprintf(buffer, sizeof(buffer), "bola seleccionada: (%.1f, %.1f)",
                selected->pos.x, selected->pos.y);
            renderer->drawText(buffer, 10, 70, textColor, font);
        }
    }

    SDL_RenderPresent(renderer->renderer);
}

void PhysicsEngine::quit() {
    // cleanup
    TTF_Quit();
    SDL_DestroyRenderer(renderer->renderer);
    SDL_DestroyWindow(sdlWindow);
    SDL_Quit();
}