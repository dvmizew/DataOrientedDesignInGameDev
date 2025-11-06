#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <random>
#include <cstring>
#include <fstream>
#include "../lib/PerformanceMonitor.h"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define DRAGAN_W (SCREEN_WIDTH / 6)
#define DRAGAN_H (SCREEN_HEIGHT / 6)

struct Spirite
{
    float x, y;
    float w, h;
    float vx, vy;
};

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

    const std::string spirite_path = "../assets/img/dragan.png";
    SDL_Texture* texture = IMG_LoadTexture(renderer, spirite_path.c_str());
    if (!texture)
    {
        std::cerr << "Texture error: " << SDL_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    Spirite dragan{rand_float(0, SCREEN_WIDTH - DRAGAN_W),
                    rand_float(0, SCREEN_HEIGHT - DRAGAN_H),
                    DRAGAN_W, DRAGAN_H,
                    200, 180};

    PerformanceMonitor perf;
    PerformanceMonitor_Init(&perf, renderer, font);

    float speed = 300.0f; // speed in pixels / second
    float angle = rand_float(0.0f, 2.0f * static_cast<float>(M_PI)); // random direction like that DVD screensaver
    dragan.vx = cosf(angle) * speed;
    dragan.vy = sinf(angle) * speed;

    bool running = true;
    SDL_Event e;

    while (running)
    {
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_EVENT_QUIT)
            {
                running = false;
            }
        }

        // update la pozitia draganului
        dragan.x += dragan.vx * (1.0f / 60.0f);
        dragan.y += dragan.vy * (1.0f / 60.0f);

        // il lovim pe dragan de marginile ecranului
        bool collided = false;
        if (dragan.x <= 0.0f)
        {
            dragan.x = 0.0f;
            dragan.vx = fabsf(dragan.vx); // inversez
            collided = true;
        }
        if (dragan.y <= 0)
        {
            dragan.y = 0.0f;
            dragan.vy = fabsf(dragan.vy);
            collided = true;
        }
        if (dragan.x + dragan.w >= SCREEN_WIDTH)
        {
            dragan.x = SCREEN_WIDTH - dragan.w;
            dragan.vx = -fabsf(dragan.vx); // inversez pe x
            collided = true;
        }
        if (dragan.y + dragan.h >= SCREEN_HEIGHT)
        {
            dragan.y = SCREEN_HEIGHT - dragan.h;
            dragan.vy = -fabsf(dragan.vy);
            collided = true;
        }

        PerformanceMonitor_Update(&perf, renderer, font);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // dragan
        SDL_FRect dst{dragan.x, dragan.y, dragan.w, dragan.h};
        SDL_RenderTexture(renderer, texture, nullptr, &dst);

        PerformanceMonitor_Draw(&perf, renderer);

        SDL_RenderPresent(renderer);
        SDL_Delay(8); // pentru FPS limit
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