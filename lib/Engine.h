#ifndef DATAORIENTEDDESIGNINGAMEDEV_ENGINE_H
#define DATAORIENTEDDESIGNINGAMEDEV_ENGINE_H

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "PerformanceMonitor.h"
#include <vector>
#include <unordered_map>

struct InputState {
    std::unordered_map<SDL_Keycode, bool> keys;
    bool quit = false;
};

class GameEngine {
protected:
    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font* font;

    std::vector<SDL_Texture*> textures;
    InputState input;
    PerformanceMonitor perf{};

    int screenWidth, screenHeight;
    float deltaTime;
    Uint64 lastFrameCounter;
    bool running;


public:
    GameEngine(int width, int height, const char* title);
    virtual ~GameEngine();

    bool initialize(const char* fontPath, const std::vector<const char*>& texturePaths);
    bool isRunning() const { return running && !input.quit; }

    void processInput();
    void update(float dt);
    void render();
    void loop();
    void shutdown();

    SDL_Renderer* getRenderer() const { return renderer; }
    TTF_Font* getFont() const { return font; }
    SDL_Texture* getTexture(const int id) const {
        return id >= 0 && id < static_cast<int>(textures.size()) ? textures[id] : nullptr;
    }
    int getScreenWidth() const { return screenWidth; }
    int getScreenHeight() const { return screenHeight; }
    float getDeltaTime() const { return deltaTime; }
    const InputState& getInput() const { return input; }

    int loadTexture(const char* path);
    void setMonitoredParticleCount(const size_t count) { perf.monitored_count = count; }

protected:
    virtual void onUpdate(float dt) {}
    virtual void onRender() {}
};

#endif