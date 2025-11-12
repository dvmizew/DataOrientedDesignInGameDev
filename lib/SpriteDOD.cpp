//
// Created by dvmi on 11/12/25.
//

#include "SpriteDOD.h"
#include <cmath>
#include <random>
#include <utility>

static float rand_float(float a, const float b)
{
    static std::mt19937 rng((unsigned)std::random_device{}());
    std::uniform_real_distribution<float> dist(a, b);
    return dist(rng);
}

SpriteManagerDOD::SpriteManagerDOD(const int screen_width, const int screen_height, const int cell_size, const float sprite_w, const float sprite_h)
    : w(sprite_w), h(sprite_h), screen_width(screen_width), screen_height(screen_height), cell_size(cell_size)
{
    grid_w = (screen_width + cell_size - 1) / cell_size;
    grid_h = (screen_height + cell_size - 1) / cell_size;
    grid.resize(grid_w * grid_h);
}

void SpriteManagerDOD::addSprite(const float pos_x, const float pos_y, const float vel_x, const float vel_y)
{
    x.push_back(pos_x);
    y.push_back(pos_y);
    vx.push_back(vel_x);
    vy.push_back(vel_y);
}

void SpriteManagerDOD::clearSprites()
{
    x.clear();
    y.clear();
    vx.clear();
    vy.clear();
}

void SpriteManagerDOD::doubleSprites(const unsigned int max_count)
{
    unsigned int new_count = x.size() * 2;
    new_count = (new_count > max_count) ? max_count : new_count;

    for (unsigned int i = x.size(); i < new_count; ++i)
    {
        const float angle = rand_float(0.0f, 2.0f * static_cast<float>(M_PI));
        constexpr float speed = 300.0f;
        addSprite(
            rand_float(0, static_cast<float>(screen_width) - w),
            rand_float(0, static_cast<float>(screen_height) - h),
            cosf(angle) * speed,
            sinf(angle) * speed
        );
    }
}

void SpriteManagerDOD::halveSprites()
{
    if (x.size() > 1)
    {
        const size_t new_size = x.size() / 2;
        x.resize(new_size);
        y.resize(new_size);
        vx.resize(new_size);
        vy.resize(new_size);
    }
}

void SpriteManagerDOD::update(const float dt)
{
    const size_t count = x.size();

    for (size_t i = 0; i < count; ++i)
    {
        x[i] += vx[i] * dt;
        y[i] += vy[i] * dt;
    }

    for (size_t i = 0; i < count; ++i)
    {
        if (x[i] <= 0.0f)
        {
            x[i] = 0.0f;
            vx[i] = fabsf(vx[i]);
        }
        if (y[i] <= 0.0f)
        {
            y[i] = 0.0f;
            vy[i] = fabsf(vy[i]);
        }
        if (x[i] + w >= static_cast<float>(screen_width))
        {
            x[i] = static_cast<float>(screen_width) - w;
            vx[i] = -fabsf(vx[i]);
        }
        if (y[i] + h >= static_cast<float>(screen_height))
        {
            y[i] = static_cast<float>(screen_height) - h;
            vy[i] = -fabsf(vy[i]);
        }
    }

    for (auto& cell : grid)
        cell.clear();

    for (size_t i = 0; i < count; ++i)
    {
        const int gx = static_cast<int>(x[i]) / cell_size;
        const int gy = static_cast<int>(y[i]) / cell_size;
        int index = gy * grid_w + gx;

        if (index >= 0 && index < static_cast<int>(grid.size()))
            grid[index].push_back(i);
    }

    for (int gy = 0; gy < grid_h; ++gy)
    {
        for (int gx = 0; gx < grid_w; ++gx)
        {
            const int index = gy * grid_w + gx;
            auto& cell = grid[index];

            for (size_t i = 0; i < cell.size(); ++i)
            {
                const size_t a = cell[i];

                for (size_t j = i + 1; j < cell.size(); ++j)
                {
                    const size_t b = cell[j];

                    const bool overlap =
                        x[a] < x[b] + w &&
                        x[a] + w > x[b] &&
                        y[a] < y[b] + h &&
                        y[a] + h > y[b];

                    if (overlap)
                        std::swap(vx[a], vx[b]), std::swap(vy[a], vy[b]);
                }
            }
        }
    }
}

void SpriteManagerDOD::render(SDL_Renderer* renderer, SDL_Texture* texture) const
{
    const size_t count = x.size();
    for (size_t i = 0; i < count; ++i)
    {
        SDL_FRect dst{x[i], y[i], w, h};
        SDL_RenderTexture(renderer, texture, nullptr, &dst);
    }
}
