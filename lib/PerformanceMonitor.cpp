//
// Created by dvmi on 11/6/25.
//

#include <iostream>
#include "PerformanceMonitor.h"

static void updateText(SDL_Renderer* renderer, TTF_Font* font, Text* text, const char* new_text, SDL_Color color)
{
    if (!new_text || strlen(new_text) == 0)
        return;
    if (strcmp(text->last_text, new_text) == 0)
        return;
    strcpy(text->last_text, new_text);
    if (text->texture)
        SDL_DestroyTexture(text->texture);

    SDL_Surface* surface = TTF_RenderText_Solid(font, new_text, strlen(new_text), color);
    if (!surface)
        return;

    text->texture = SDL_CreateTextureFromSurface(renderer, surface);
    text->w = surface->w;
    text->h = surface->h;
    SDL_DestroySurface(surface);
}

static void DrawText(SDL_Renderer* renderer, Text* textObj, int x, int y)
{
    if (!textObj->texture) return;
    SDL_FRect dst = {(float)x, (float)y, (float)textObj->w, (float)textObj->h};
    SDL_RenderTexture(renderer, textObj->texture, nullptr, &dst);
}

void PerformanceMonitor_Init(PerformanceMonitor* pm, SDL_Renderer* renderer, TTF_Font* font)
{
    memset(pm, 0, sizeof(PerformanceMonitor));
    pm->freq = SDL_GetPerformanceFrequency();
    pm->last_counter = SDL_GetPerformanceCounter();
    pm->color = (SDL_Color){255, 255, 255, 255};
}

void PerformanceMonitor_Update(PerformanceMonitor* pm, SDL_Renderer* renderer, TTF_Font* font)
{
    Uint64 now = SDL_GetPerformanceCounter();
    double dt = static_cast<double>(now - pm->last_counter) / static_cast<double>(pm->freq);
    pm->last_counter = now;

    float fps = (dt > 0.0) ? static_cast<float>(1.0 / dt) : 0.0f;
    pm->fps_samples[pm->fps_index] = fps;
    pm->fps_index = (pm->fps_index + 1) % FPS_SAMPLES;

    float sum = 0.0f;
    for (float fps_sample : pm->fps_samples)
        sum += fps_sample;
    pm->avg_fps = sum / FPS_SAMPLES;
    pm->frame_time_ms = static_cast<float>(dt * 1000.0);

    char fps_line[64], frame_line[64], mem_line[64];
    snprintf(fps_line, sizeof(fps_line), "FPS: %.1f", pm->avg_fps);
    snprintf(frame_line, sizeof(frame_line), "Frame: %.3f ms", pm->frame_time_ms);

    SDL_PropertiesID props = SDL_GetGlobalProperties();
    Uint64 texture_mem = SDL_GetNumberProperty(props, "SDL.video.texture_memory_usage", 0);
    snprintf(mem_line, sizeof(mem_line), "Memory: %.2f MB", texture_mem / (1024.0f * 1024.0f));

    updateText(renderer, font, &pm->fps_text, fps_line, pm->color);
    updateText(renderer, font, &pm->frame_text, frame_line, pm->color);
    updateText(renderer, font, &pm->mem_text, mem_line, pm->color);
}

void PerformanceMonitor_Draw(PerformanceMonitor* pm, SDL_Renderer* renderer)
{
    DrawText(renderer, &pm->fps_text, 10, 10);
    DrawText(renderer, &pm->frame_text, 10, 40);
    DrawText(renderer, &pm->mem_text, 10, 70);
}

void PerformanceMonitor_Destroy(PerformanceMonitor* pm)
{
    if (pm->fps_text.texture) SDL_DestroyTexture(pm->fps_text.texture);
    if (pm->frame_text.texture) SDL_DestroyTexture(pm->frame_text.texture);
    if (pm->mem_text.texture) SDL_DestroyTexture(pm->mem_text.texture);
}