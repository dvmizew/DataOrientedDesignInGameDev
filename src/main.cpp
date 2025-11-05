#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <random>
#include <cstring>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define DRAGAN_W (SCREEN_WIDTH / 2)
#define DRAGAN_H (SCREEN_HEIGHT / 2)

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

    const std::string spirite_path = "../assets/dragan.png";
    SDL_Texture* texture = IMG_LoadTexture(renderer, spirite_path.c_str());
    if (!texture)
    {
        std::cerr << "Texture error: " << SDL_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    Spirite dragan;
    dragan.w = static_cast<float>(DRAGAN_W);
    dragan.h = static_cast<float>(DRAGAN_H);

    // il scalez pe dragan sa nu fie prea mare
    const float max_scale = 200.0f;
    if (dragan.w > max_scale)
    {
        float scale = max_scale / dragan.w;
        dragan.w *= scale;
        dragan.h *= scale;
    }

    dragan.x = rand_float(0.0f, SCREEN_WIDTH - dragan.w);
    dragan.y = rand_float(0.0f, SCREEN_HEIGHT - dragan.h);

    float speed = 300.0f; // speed in pixels / second
    float angle = rand_float(0.0f, 2.0f * M_PIf); // random direction like that DVD screensaver
    dragan.vx = cosf(angle) * speed;
    dragan.vy = sinf(angle) * speed;

    bool running = true;
    SDL_Event e;

    uint64_t freq = SDL_GetPerformanceFrequency();
    uint64_t last = SDL_GetPerformanceCounter();

    while (running)
    {
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_EVENT_QUIT)
            {
                running = false;
            }
        }
        // update delta time
        uint64_t now = SDL_GetPerformanceCounter();
        float dt = static_cast<float>(now - last) / static_cast<float>(freq);
        last = now;

        // update la pozitia draganului
        dragan.x += dragan.vx * dt;
        dragan.y += dragan.vy * dt;

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

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // black background
        SDL_RenderClear(renderer);

        SDL_FRect dst{dragan.x, dragan.y, dragan.w, dragan.h}; // sprite zone
        SDL_RenderTexture(renderer, texture, nullptr, &dst);

        SDL_RenderPresent(renderer);

        SDL_Delay(16);
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}