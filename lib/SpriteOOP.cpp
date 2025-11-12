//
// Created by dvmi on 11/12/25.
//

#include "SpriteOOP.h"
#include <cmath>
#include <random>

static float rand_float(float a, float b)
{
    static std::mt19937 rng((unsigned)std::random_device{}());
    std::uniform_real_distribution<float> dist(a, b);
    return dist(rng);
}

SpriteManagerOOP::SpriteManagerOOP(const int screen_width, const int screen_height, const int cell_size)
    : screen_width(screen_width), screen_height(screen_height), cell_size(cell_size)
{
    grid_w = (screen_width + cell_size - 1) / cell_size;
    grid_h = (screen_height + cell_size - 1) / cell_size;
    grid.resize(grid_w * grid_h);
}

void SpriteManagerOOP::addSprite(const float x, const float y, float w, const float h, const float vx, const float vy)
{
    sprites.push_back({x, y, w, h, vx, vy});
}

void SpriteManagerOOP::clearSprites()
{
    sprites.clear();
}

void SpriteManagerOOP::doubleSprites(const float sprite_w, const float sprite_h, const unsigned int max_count)
{
    unsigned int new_count = sprites.size() * 2;
    new_count = (new_count > max_count) ? max_count : new_count;

    for (unsigned int i = sprites.size(); i < new_count; ++i)
    {
        const float angle = rand_float(0.0f, 2.0f * static_cast<float>(M_PI));
        constexpr float speed = 300.0f;
        addSprite(
            rand_float(0, static_cast<float>(screen_width) - sprite_w),
            rand_float(0, static_cast<float>(screen_height) - sprite_h),
            sprite_w, sprite_h,
            cosf(angle) * speed,
            sinf(angle) * speed
        );
    }
}

void SpriteManagerOOP::halveSprites()
{
    if (sprites.size() > 1)
    {
        size_t new_size = sprites.size() / 2;
        sprites.erase(sprites.begin() + new_size, sprites.end());
    }
}

void SpriteManagerOOP::update(const float dt)
{
    for (auto& s : sprites)
    {
        s.x += s.vx * dt;
        s.y += s.vy * dt;

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
        if (s.x + s.w >= static_cast<float>(screen_width))
        {
            s.x = static_cast<float>(screen_width) - s.w;
            s.vx = -fabsf(s.vx);
        }
        if (s.y + s.h >= static_cast<float>(screen_height))
        {
            s.y = static_cast<float>(screen_height) - s.h;
            s.vy = -fabsf(s.vy);
        }
    }

    for (auto& cell : grid)
        cell.clear();

    for (size_t i = 0; i < sprites.size(); ++i)
    {
        const Sprite& s = sprites[i];
        const int gx = static_cast<int>(s.x) / cell_size;
        const int gy = static_cast<int>(s.y) / cell_size;

        if (const int index = gy * grid_w + gx; index >= 0 && index <static_cast<int>(grid.size()))
            grid[index].push_back(i);
    }

    // sprite collisions
    for (int y = 0; y < grid_h; ++y)
    {
        for (int x = 0; x < grid_w; ++x)
        {
            const int index = y * grid_w + x;
            auto& cell = grid[index];

            for (size_t i = 0; i < cell.size(); ++i)
            {
                const size_t a_index = cell[i];
                Sprite& a = sprites[a_index];

                for (size_t j = i + 1; j < cell.size(); ++j)
                {
                    size_t b_index = cell[j];
                    Sprite& b = sprites[b_index];

                    bool overlap =
                        a.x < b.x + b.w &&
                        a.x + a.w > b.x &&
                        a.y < b.y + b.h &&
                        a.y + a.h > b.y;

                    if (overlap)
                        std::swap(a.vx, b.vx), std::swap(a.vy, b.vy);
                }
            }
        }
    }
}

void SpriteManagerOOP::render(SDL_Renderer* renderer, SDL_Texture* texture) const
{
    for (const auto& s : sprites)
    {
        SDL_FRect dst{s.x, s.y, s.w, s.h};
        SDL_RenderTexture(renderer, texture, nullptr, &dst);
    }
}