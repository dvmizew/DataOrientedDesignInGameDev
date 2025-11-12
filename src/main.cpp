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
#define DRAGAN_W (SCREEN_WIDTH / 50)
#define DRAGAN_H (SCREEN_HEIGHT / 50)

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

    std::vector<Spirite> spirites;
    unsigned int spirites_count = 1000;
    for (unsigned int i = 0; i < spirites_count; ++i)
    {
        float angle = rand_float(0.0f, 2.0f * static_cast<float>(M_PI));
        float speed = 300.0f;
        spirites.push_back({
            rand_float(0, SCREEN_WIDTH - DRAGAN_W),
            rand_float(0, SCREEN_HEIGHT - DRAGAN_H),
                DRAGAN_W, DRAGAN_H,
                cosf(angle) * speed,
                sinf(angle) * speed
        });
    }

    PerformanceMonitor perf;
    PerformanceMonitor_Init(&perf, renderer, font);
    bool running = true;
    SDL_Event e;

    constexpr int CELL_SIZE = DRAGAN_W * 2;
    int grid_w = (SCREEN_WIDTH + CELL_SIZE - 1) / CELL_SIZE;
    int grid_h = (SCREEN_HEIGHT + CELL_SIZE - 1) / CELL_SIZE;

    std::vector<std::vector<size_t>> grid(grid_w * grid_h);
    while (running)
    {
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_EVENT_QUIT)
            {
                running = false;
            }

            if (e.type == SDL_EVENT_QUIT)
                running = false;

            if (e.type == SDL_EVENT_KEY_DOWN)
            {
                // UP key doubles the spirite on the screen
                if (e.key.key == SDLK_UP && spirites.size() < 100000)
                {
                    unsigned int new_count = spirites.size() * 2;
                    new_count = (new_count > 100000) ? 100000 : new_count;

                    for (unsigned int i = spirites.size(); i < new_count; ++i)
                    {
                        const float angle = rand_float(0.0f, 2.0f * static_cast<float>(M_PI));
                        const float speed = 300.0f;
                        spirites.push_back({
                            rand_float(0, SCREEN_WIDTH - DRAGAN_W),
                            rand_float(0, SCREEN_HEIGHT - DRAGAN_H),
                            DRAGAN_W, DRAGAN_H,
                            cosf(angle) * speed,
                            sinf(angle) * speed
                            });
                    }
                    // DOWN key reduces them in half
                } else if (e.key.key == SDLK_DOWN && spirites.size() > 1)
                    spirites.resize(spirites.size() / 2);
            }
        }

        static Uint64 last_counter = SDL_GetPerformanceCounter();

        Uint64 now = SDL_GetPerformanceCounter();
        float dt = static_cast<float>(now - last_counter) / static_cast<float>(SDL_GetPerformanceFrequency());
        last_counter = now;

        for (auto& s: spirites)
        {
            s.x += s.vx * dt;
            s.y += s.vy * dt;

            // collisions
            if (s.x <= 0.0f)
            {
                s.x = 0.0f;
                s.vx = fabsf(s.vx);
            }
            if (s.y <= 0.0f)
            {
                s.y = 0.0f;
                s.vy = fabsf(s.vy);
            }
            if (s.x + s.w >= SCREEN_WIDTH)
            {
                s.x = SCREEN_WIDTH - s.w;
                s.vx = -fabsf(s.vx);
            }
            if (s.y + s.h >= SCREEN_HEIGHT)
            {
                s.y = SCREEN_HEIGHT - s.h;
                s.vy = -fabsf(s.vy);
            }
        }

        for (auto &cell : grid)
            cell.clear();

        // adding the spirits into the grid
        size_t spirites_size = spirites.size();
        for (size_t i = 0; i < spirites_size; ++i)
        {
            Spirite& s = spirites[i];
            int x = s.x / CELL_SIZE;
            int y = s.y / CELL_SIZE;
            int index = y * grid_w + x;

            if (index >= 0 && index < grid.size())
                grid[index].push_back(i);
        }

        // spirite collision in the same grid
        for (int y = 0; y < grid_h; ++y)
        {
            for (int x = 0; x < grid_w; ++x)
            {
                int index = y * grid_w + x;
                auto& cell = grid[index];

                size_t cell_size = cell.size();
                for (size_t i = 0; i < cell_size; ++i)
                {
                    size_t a_index = cell[i];
                    Spirite& a = spirites[a_index];

                    for (size_t j = i + 1; j < cell_size; ++j)
                    {
                        size_t b_index = cell[j];
                        Spirite &b = spirites[b_index];

                        bool overlap =
                            a.x < b.x + b.w &&
                                a.x +a.w > b.x &&
                                    a.y < b.y + b.h &&
                                        a.y + a.h > b.y;

                        if (overlap)
                            std::swap(a.vx, b.vx), std::swap(a.vy, b.vy);
                    }
                }
            }
        }

        PerformanceMonitor_Update(&perf, renderer, font, spirites.size());

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        for (const auto &s : spirites)
        {
            SDL_FRect dst{s.x, s.y, s.w, s.h};
            SDL_RenderTexture(renderer, texture, nullptr, &dst);
        }

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