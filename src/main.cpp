#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <random>
#include <cstring>
#include <fstream>
// for performance monitor
#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#else
#include <unistd.h>
#endif

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define DRAGAN_W 500
#define DRAGAN_H 450
#define FPS_SAMPLES 100

struct Spirite
{
    float x, y;
    float w, h;
    float vx, vy;
};

struct Text
{
    SDL_Texture* texture = nullptr;
    int w = 0, h = 0;
    static constexpr size_t MAX_TEXT = 128;
    char last_text_buffer[MAX_TEXT] = {0};

    void update(SDL_Renderer* renderer, TTF_Font* font, const char* text, size_t text_len, SDL_Color color)
    {
        if (!text || text_len == 0)
            return;

        if (text_len >= MAX_TEXT)
            text_len = MAX_TEXT - 1;

        if (std::memcmp(last_text_buffer, text, text_len) == 0 && last_text_buffer[text_len] == '\0')
            return;

        std::memcpy(last_text_buffer, text, text_len);
        last_text_buffer[text_len] = '\0';

        if (texture)
            SDL_DestroyTexture(texture);

        SDL_Surface* surface = TTF_RenderText_Solid(font, text, text_len, color);
        if (!surface)
        {
            texture = nullptr;
            w = h = 0;
            return;
        }

        texture = SDL_CreateTextureFromSurface(renderer, surface);
        w = surface->w;
        h = surface->h;
        SDL_DestroySurface(surface);
    }

    void draw(SDL_Renderer* renderer, int x, int y)
    {
        if (!texture)
            return;
        SDL_FRect dst{float(x), float(y), float(w), float(h)};
        SDL_RenderTexture(renderer, texture, nullptr, &dst);
    }
    ~Text()
    {
        if (texture) SDL_DestroyTexture(texture);
    }
};

static float rand_float(float a, float b)
{
    static std::mt19937 rng((unsigned)std::random_device{}());
    std::uniform_real_distribution<float> dist(a, b);
    return dist(rng);
}

float fps = 0.0f;
float avg_fps = 0.0f;
float frame_time_ms = 0.0f;
float fps_samples[FPS_SAMPLES] = {0};
int fps_index = 0;

TTF_Font* font = nullptr;

float getMemoryUsage()
{
#ifdef _WIN32
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc)))
    {
        return static_cast<float>(pmc.WorkingSetSize) / (1024.0f * 1024.0f);
    }
    return -1.0f;
#else

    std::ifstream statm("/proc/self/statm"); // the linux way
    long size = 0, resident = 0;
    if (statm >> size >> resident)
    {
        long page_size_kb = __sysconf(_SC_PAGE_SIZE) / 1024; // KB
        return (resident * page_size_kb) / 1024.0f;
    }
    return -1;
#endif
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

    font = TTF_OpenFont("../assets/fonts/Roboto.ttf", 20);
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

    Spirite dragan{};
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
    float angle = rand_float(0.0f, 2.0f * (float)M_PI); // random direction like that DVD screensaver
    dragan.vx = cosf(angle) * speed;
    dragan.vy = sinf(angle) * speed;

    bool running = true;
    SDL_Event e;

    uint64_t freq = SDL_GetPerformanceFrequency();
    uint64_t last = SDL_GetPerformanceCounter();

    SDL_Color white = {255, 255, 255, 255};
    char line1[64], line2[64], line3[64];
    Text fps_text;
    Text frame_text;
    Text mem_text;

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

        // calcul FPS
        fps = 1.0f / dt;
        frame_time_ms = dt * 1000.0f;
        fps_samples[fps_index] = fps;
        fps_index = (fps_index + 1) % FPS_SAMPLES;

        // calcul avg
        float sum = 0.0f;
        for (float fps_sample : fps_samples)
            sum += fps_sample;
        avg_fps = sum / FPS_SAMPLES;

        // calcul utilizare memorie
        float mem_usage_mb = getMemoryUsage();

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

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // dragan
        SDL_FRect dst{dragan.x, dragan.y, dragan.w, dragan.h};
        SDL_RenderTexture(renderer, texture, nullptr, &dst);

        // text
        snprintf(line1, sizeof(line1), "FPS: %.2f", avg_fps);
        snprintf(line2, sizeof(line2), "Frame time: %.3f ms", frame_time_ms);
        snprintf(line3, sizeof(line3), "Memory: %.2f MB", mem_usage_mb);

        fps_text.update(renderer, font, line1, strlen(line1), white);
        frame_text.update(renderer, font, line2, strlen(line2), white);
        mem_text.update(renderer, font, line3, strlen(line3), white);

        fps_text.draw(renderer, 10, 10);
        frame_text.draw(renderer, 10, 40);
        mem_text.draw(renderer, 10, 70);

        SDL_RenderPresent(renderer);
        // SDL_Delay(8); // pentru FPS limit
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();
    return 0;
}