//
// Created by dvmi on 11/6/25.
//

#include <iostream>
#include <fstream>
#include "PerformanceMonitor.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <psapi.h>
#endif

static void updateText(SDL_Renderer* renderer, TTF_Font* font, Text* text, const char* new_text, const SDL_Color color)
{
    if (!new_text || strlen(new_text) == 0)
        return;
    if (strcmp(text->last_text, new_text) == 0)
        return;
    strncpy(text->last_text, new_text, sizeof(text->last_text) - 1);
    text->last_text[sizeof(text->last_text) - 1] = '\0';
    if (text->texture)
        SDL_DestroyTexture(text->texture);

    SDL_Surface* surface = TTF_RenderText_Solid(font, new_text, strlen(new_text), color);
    if (!surface)
        return;

    text->texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (text->texture) {
        text->w = surface->w;
        text->h = surface->h;
    }
    SDL_DestroySurface(surface);
}

static void DrawText(SDL_Renderer* renderer, const Text* textObj, int x, const int y)
{
    if (!renderer || !textObj || !textObj->texture) return;
    const SDL_FRect dst = {static_cast<float>(x), static_cast<float>(y), static_cast<float>(textObj->w), static_cast<float>(textObj->h)};
    SDL_RenderTexture(renderer, textObj->texture, nullptr, &dst);
}

size_t getMemoryMB()
{
#ifdef __linux__
    std::ifstream status("/proc/self/status");
    if (!status.is_open())
        return 0;
    std::string line;
    while (std::getline(status, line))
    {
        if (line.size() > 6 && line.substr(0, 6) == "VmRSS:") {
            const std::string value_str = line.substr(6);
            char* endptr = nullptr;
            const long value = strtol(value_str.c_str(), &endptr, 10);
            if (endptr != value_str.c_str() && value > 0) {
                return static_cast<size_t>(value) / 1024;
            }
        }
    }
#elif defined(_WIN32)
    PROCESS_MEMORY_COUNTERS_EX pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&pmc), sizeof(pmc)))
    {
        const size_t rss_bytes = static_cast<size_t>(pmc.WorkingSetSize);
        return rss_bytes / (1024 * 1024);
    }
#endif
    return 0;
}

void PerformanceMonitor_Init(PerformanceMonitor* pm, SDL_Renderer* renderer, TTF_Font* font)
{
    memset(pm, 0, sizeof(PerformanceMonitor));
    pm->freq = SDL_GetPerformanceFrequency();
    pm->last_counter = SDL_GetPerformanceCounter();
    pm->color = (SDL_Color){255, 255, 255, 255};
    pm->monitored_count = 0;
}

void PerformanceMonitor_Update(PerformanceMonitor* pm, SDL_Renderer* renderer, TTF_Font* font, size_t sprite_count)
{
    const Uint64 now = SDL_GetPerformanceCounter();
    const double dt = static_cast<double>(now - pm->last_counter) / static_cast<double>(pm->freq);
    pm->last_counter = now;

    const float fps = (dt > 0.0) ? static_cast<float>(1.0 / dt) : 0.0f;
    pm->fps_samples[pm->fps_index] = fps;
    pm->fps_index = (pm->fps_index + 1) % FPS_SAMPLES;

    float sum = 0.0f;
    for (const float fps_sample : pm->fps_samples)
        sum += fps_sample;
    pm->avg_fps = sum / FPS_SAMPLES;
    pm->frame_time_ms = static_cast<float>(dt * 1000.0);

    const size_t final_count = (pm->monitored_count > 0) ? pm->monitored_count : sprite_count;

    char fps_line[64], frame_line[64], mem_line[64], sprite_line[64];
    snprintf(fps_line, sizeof(fps_line), "FPS: %.1f", pm->avg_fps);
    snprintf(frame_line, sizeof(frame_line), "Frame: %.3f ms", pm->frame_time_ms);

    const size_t mem_mb = getMemoryMB();
    snprintf(mem_line, sizeof(mem_line), "Memory: %zu MB", mem_mb);
    snprintf(sprite_line, sizeof(sprite_line), "Sprites: %zu", final_count);
    updateText(renderer, font, &pm->fps_text, fps_line, pm->color);
    updateText(renderer, font, &pm->frame_text, frame_line, pm->color);
    updateText(renderer, font, &pm->mem_text, mem_line, pm->color);
    updateText(renderer, font, &pm->sprite_count_text, sprite_line, pm->color);
}

void PerformanceMonitor_Draw(const PerformanceMonitor* pm, SDL_Renderer* renderer)
{
    DrawText(renderer, &pm->fps_text, 10, 10);
    DrawText(renderer, &pm->frame_text, 10, 40);
    DrawText(renderer, &pm->mem_text, 10, 70);
    DrawText(renderer, &pm->sprite_count_text, 10,100);
}

void PerformanceMonitor_Destroy(const PerformanceMonitor* pm)
{
    if (pm->fps_text.texture) SDL_DestroyTexture(pm->fps_text.texture);
    if (pm->frame_text.texture) SDL_DestroyTexture(pm->frame_text.texture);
    if (pm->mem_text.texture) SDL_DestroyTexture(pm->mem_text.texture);
    if (pm->sprite_count_text.texture) SDL_DestroyTexture(pm->sprite_count_text.texture);
}