//
// Created by dvmi on 11/12/25.
//

#ifndef DATAORIENTEDDESIGNINGAMEDEV_SPRITEDOD_H
#define DATAORIENTEDDESIGNINGAMEDEV_SPRITEDOD_H

#include <SDL3/SDL.h>
#include <vector>

struct SpriteManagerDOD
{
    // position
    std::vector<float> x;
    std::vector<float> y;

    // velocity
    std::vector<float> vx;
    std::vector<float> vy;

    // size
    float w;
    float h;

    int screen_width;
    int screen_height;

    int cell_size;
    int grid_w;
    int grid_h;
    std::vector<std::vector<size_t>> grid;

    SpriteManagerDOD(int screen_width, int screen_height, int cell_size, float sprite_w, float sprite_h);

    void addSprite(float pos_x, float pos_y, float vel_x, float vel_y);
    void clearSprites();
    void doubleSprites(unsigned int max_count = 100000);
    void halveSprites();

    void update(float dt);
    void render(SDL_Renderer* renderer, SDL_Texture* texture) const;

    size_t getCount() const { return x.size(); }
};

#endif //DATAORIENTEDDESIGNINGAMEDEV_SPRITEDOD_H