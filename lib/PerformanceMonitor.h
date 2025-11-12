//
// Created by dvmi on 11/6/25.
//

#ifndef DATAORIENTEDDESIGNINGAMEDEV_PERFORMANCEMONITOR_H
#define DATAORIENTEDDESIGNINGAMEDEV_PERFORMANCEMONITOR_H

#define FPS_SAMPLES 100
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

typedef struct
{
    SDL_Texture* texture;
    int w, h;
    char last_text[128];
}Text;

typedef struct
{
    float fps_samples[FPS_SAMPLES];
    int fps_index;
    float avg_fps;
    float frame_time_ms;
    Uint64 last_counter;
    Uint64 freq;

    Text fps_text;
    Text frame_text;
    Text mem_text;
    Text spirite_count_text;
    SDL_Color color;
}PerformanceMonitor;

size_t getMemoryMB();
static void updateText(SDL_Renderer* renderer, TTF_Font* font, Text* text, const char* new_text, SDL_Color color);
void PerformanceMonitor_Init(PerformanceMonitor* pm, SDL_Renderer* renderer, TTF_Font* font);
void PerformanceMonitor_Update(PerformanceMonitor* pm, SDL_Renderer* renderer, TTF_Font* font, size_t spirite_count);
void PerformanceMonitor_Draw(const PerformanceMonitor* pm, SDL_Renderer* renderer);
void PerformanceMonitor_Destroy(const PerformanceMonitor* pm);

#endif //DATAORIENTEDDESIGNINGAMEDEV_PERFORMANCEMONITOR_H