#include "Engine.h"
#include <SDL3_image/SDL_image.h>
#include <iostream>

GameEngine::GameEngine(const int width, const int height, const char* title)
    : window(nullptr), renderer(nullptr), font(nullptr),
      screenWidth(width), screenHeight(height), deltaTime(0),
      lastFrameCounter(0), running(false) {

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
        return;
    }

    if (!TTF_Init()) {
        std::cerr << "TTF_Init failed: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return;
    }

    window = SDL_CreateWindow(title, width, height, 0);
    if (!window) {
        std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
        TTF_Quit();
        SDL_Quit();
        return;
    }

    renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        std::cerr << "Renderer creation failed: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return;
    }

    lastFrameCounter = SDL_GetPerformanceCounter();
    running = true;
}

GameEngine::~GameEngine() {
    shutdown();
}

bool GameEngine::initialize(const char* fontPath, const std::vector<const char*>& texturePaths) {
    font = TTF_OpenFont(fontPath, 20);
    if (!font) {
        std::cerr << "Font loading failed: " << SDL_GetError() << std::endl;
        return false;
    }

    PerformanceMonitor_Init(&perf, renderer, font);

    for (const char* path : texturePaths) {
        int id = loadTexture(path);
        if (id < 0) {
            std::cerr << "Failed to load texture: " << path << std::endl;
            return false;
        }
    }

    return true;
}

int GameEngine::loadTexture(const char* path) {
    SDL_Texture* texture = IMG_LoadTexture(renderer, path);
    if (!texture) {
        std::cerr << "Texture load failed: " << path << std::endl;
        return -1;
    }
    textures.push_back(texture);
    return static_cast<int>(textures.size()) - 1;
}

void GameEngine::processInput() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_EVENT_QUIT) {
            input.quit = true;
            running = false;
        }

        if (e.type == SDL_EVENT_KEY_DOWN) {
            input.keys[e.key.key] = true;
        }

        if (e.type == SDL_EVENT_KEY_UP) {
            input.keys[e.key.key] = false;
        }
    }

    if (input.keys.count(SDLK_ESCAPE) && input.keys.at(SDLK_ESCAPE)) {
        input.quit = true;
        running = false;
    }
}

void GameEngine::update(const float dt) {
    onUpdate(dt);
}

void GameEngine::render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    onRender();

    PerformanceMonitor_Update(&perf, renderer, font, 0);
    PerformanceMonitor_Draw(&perf, renderer);

    SDL_RenderPresent(renderer);
}

void GameEngine::loop() {
    while (isRunning()) {
        const Uint64 now = SDL_GetPerformanceCounter();
        deltaTime = static_cast<float>(now - lastFrameCounter) / static_cast<float>(SDL_GetPerformanceFrequency());
        lastFrameCounter = now;

        processInput();
        update(deltaTime);
        render();
    }
}

void GameEngine::shutdown() {
    PerformanceMonitor_Destroy(&perf);

    for (const auto tex : textures) {
        SDL_DestroyTexture(tex);
    }
    textures.clear();

    if (font) {
        TTF_CloseFont(font);
    }

    if (renderer) {
        SDL_DestroyRenderer(renderer);
    }

    if (window) {
        SDL_DestroyWindow(window);
    }

    TTF_Quit();
    SDL_Quit();
}