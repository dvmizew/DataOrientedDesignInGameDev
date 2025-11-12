//
// Created by dvmi on 11/12/25.
//

#ifndef DATAORIENTEDDESIGNINGAMEDEV_SPRITEOOP_H
#define DATAORIENTEDDESIGNINGAMEDEV_SPRITEOOP_H

#include <SDL3/SDL.h>
#include <vector>

struct Sprite
{
    float x, y;
    float w, h;
    float vx, vy;
};

struct SpriteManagerOOP
{
    std::vector<Sprite> sprites;
    int screen_width;
    int screen_height;
    int cell_size;
    int grid_w;
    int grid_h;
    std::vector<std::vector<size_t>> grid;

    SpriteManagerOOP(int screen_width, int screen_height, int cell_size);

    void addSprite(float x, float y, float w, float h, float vx, float vy);
    void clearSprites();
    void doubleSprites(float sprite_w, float sprite_h, unsigned int max_count = 100000);
    void halveSprites();

    void update(float dt);
    void render(SDL_Renderer* renderer, SDL_Texture* texture) const;

    size_t getCount() const { return sprites.size(); }
};

#endif //DATAORIENTEDDESIGNINGAMEDEV_SPRITEOOP_H