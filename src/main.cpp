#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <cmath>
#include <random>
#include "../lib/PerformanceMonitor.h"
#include "../lib/SpriteOOP.h"
#include "../lib/SpriteDOD.h"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define DRAGAN_W (SCREEN_WIDTH / (float)50.0)
#define DRAGAN_H (SCREEN_HEIGHT / (float)50.0)


static float rand_float(float a, float b)
{
    static std::mt19937 rng((unsigned)std::random_device{}());
    std::uniform_real_distribution<float> dist(a, b);
    return dist(rng);
}

int main(int argc, char* argv[]) {
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
        return 1;
    }
    if (!TTF_Init())
    {
        std::cerr << "TTF_Init failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    TTF_Font* font = TTF_OpenFont("../assets/fonts/Roboto.ttf", 20);
    if (!font)
    {
        std::cerr << "Font couldn't be loaded: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Dragan Mihaita 204 - Screensaver",
        SCREEN_WIDTH, SCREEN_HEIGHT,
        0);
    if (!window)
    {
        std::cerr << "Window couldn't be created: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer)
    {
        std::cerr << "Renderer couldn't be initialized: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    const std::string sprite_path = "../assets/img/dragan.png";
    SDL_Texture* texture = IMG_LoadTexture(renderer, sprite_path.c_str());
    if (!texture)
    {
        std::cerr << "Texture error: " << SDL_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    constexpr int CELL_SIZE = DRAGAN_W * 2;
    SpriteManagerOOP spriteOOP(SCREEN_WIDTH, SCREEN_HEIGHT, CELL_SIZE);
    SpriteManagerDOD spriteDOD(SCREEN_WIDTH, SCREEN_HEIGHT, CELL_SIZE, DRAGAN_W, DRAGAN_H);

    bool useOOP = true;

    unsigned int initial_count = 1000;
    for (unsigned int i = 0; i < initial_count; ++i)
    {
        float angle = rand_float(0.0f, 2.0f * static_cast<float>(M_PI));
        float speed = 300.0f;
        float x = rand_float(0, SCREEN_WIDTH - DRAGAN_W);
        float y = rand_float(0, SCREEN_HEIGHT - DRAGAN_H);
        float vx = cosf(angle) * speed;
        float vy = sinf(angle) * speed;

        spriteOOP.addSprite(x, y, DRAGAN_W, DRAGAN_H, vx, vy);
        spriteDOD.addSprite(x, y, vx, vy);
    }

    PerformanceMonitor perf;
    PerformanceMonitor_Init(&perf, renderer, font);
    bool running = true;
    SDL_Event e;
    Uint64 last_counter = SDL_GetPerformanceCounter();
    while (running)
    {
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_EVENT_QUIT)
                running = false;

            if (e.type == SDL_EVENT_KEY_DOWN)
            {
                // UP key doubles the sprites on the screen
                if (e.key.key == SDLK_UP)
                {
                    if (useOOP && spriteOOP.getCount() < 100000)
                    {
                        spriteOOP.doubleSprites(DRAGAN_W, DRAGAN_H);
                        spriteDOD.clearSprites();
                        for (unsigned int i = 0; i < spriteOOP.getCount(); ++i)
                        {
                            float angle = rand_float(0.0f, 2.0f * static_cast<float>(M_PI));
                            float speed = 300.0f;
                            spriteDOD.addSprite(
                                rand_float(0, SCREEN_WIDTH - DRAGAN_W),
                                rand_float(0, SCREEN_HEIGHT - DRAGAN_H),
                                cosf(angle) * speed,
                                sinf(angle) * speed
                            );
                        }
                    }
                    else if (!useOOP && spriteDOD.getCount() < 100000)
                    {
                        spriteDOD.doubleSprites();
                        spriteOOP.clearSprites();
                        for (unsigned int i = 0; i < spriteDOD.getCount(); ++i)
                        {
                            float angle = rand_float(0.0f, 2.0f * static_cast<float>(M_PI));
                            float speed = 300.0f;
                            spriteOOP.addSprite(
                                rand_float(0, SCREEN_WIDTH - DRAGAN_W),
                                rand_float(0, SCREEN_HEIGHT - DRAGAN_H),
                                DRAGAN_W, DRAGAN_H,
                                cosf(angle) * speed,
                                sinf(angle) * speed
                            );
                        }
                    }
                }
                // DOWN key reduces them in half
                else if (e.key.key == SDLK_DOWN)
                {
                    spriteOOP.halveSprites();
                    spriteDOD.halveSprites();
                }
                // SPACE key toggles between OOP and DOD
                else if (e.key.key == SDLK_SPACE)
                {
                    useOOP = !useOOP;
                    std::cout << (useOOP ? "OOP" : "DOD") << std::endl;
                }
                // ESC key quits
                else if (e.key.key == SDLK_ESCAPE)
                {
                    running = false;
                }
            }
        }

        Uint64 now = SDL_GetPerformanceCounter();
        float dt = static_cast<float>(now - last_counter) / static_cast<float>(SDL_GetPerformanceFrequency());
        last_counter = now;

        if (useOOP)
            spriteOOP.update(dt);
        else
            spriteDOD.update(dt);

        size_t sprite_count = useOOP ? spriteOOP.getCount() : spriteDOD.getCount();
        PerformanceMonitor_Update(&perf, renderer, font, sprite_count);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        if (useOOP)
            spriteOOP.render(renderer, texture);
        else
            spriteDOD.render(renderer, texture);

        PerformanceMonitor_Draw(&perf, renderer);

        SDL_RenderPresent(renderer);
    }

    PerformanceMonitor_Destroy(&perf);
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();
    return 0;
}