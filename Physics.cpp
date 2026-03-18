#include "Physics.h"

#include "Color.h"
#include "Input.h"
#include "Renderer.h"

#include <SDL.h>
#include <SDL_ttf.h>

#include <cmath>
#include <iostream>
#include <memory>


int main(int argc, char** argv) {
    return PhysicsEngine::getInstance()->run(argc, argv);
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
            input->tick(sdlEvent);
        }

        int mouseX, mouseY;
        Uint32 buttons = SDL_GetMouseState(&mouseX, &mouseY);
        input->onMouseMove(mouseX, mouseY);
        
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
        } else if (auto rect = dynamic_cast<RectangleShape*>(body->shape.get())) {
            renderer->drawFilledRect(
                static_cast<int>(std::round(body->pos.x)),
                static_cast<int>(std::round(body->pos.y)),
                rect->halfWidth,
                rect->halfHeight,
                Color::GREEN
            );
        }
    }

    // texto
    if (font) {
        renderer->drawText("clic izq.: arrastrar bola | ruedita (pulsar): crear bola | clic dcho.: crear cuadrado | ruedita: cambiar tamaño",
            10, 10, textColor, font);

        char buffer[128];
        snprintf(buffer, sizeof(buffer), "tamaño: %.2f", currentRadius);
        renderer->drawText(buffer, 10, 40, textColor, font);

        // si hay una bola seleccionada, mostrar sus coordenadas
        if (input->selected) {
            char buffer[128];
            snprintf(buffer, sizeof(buffer), "seleccionado: (%.1f, %.1f)",
                input->selected->pos.x, input->selected->pos.y);
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