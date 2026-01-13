#include "Particles.h"
#include "Utils.h"
#include <cmath>

Particles::Particles(const int screen_width, const int screen_height, const int cell_size, const float sprite_w, const float sprite_h)
    : w(sprite_w), h(sprite_h), screen_width(screen_width), screen_height(screen_height), cell_size(cell_size)
{
    grid_w = (screen_width + cell_size - 1) / cell_size;
    grid_h = (screen_height + cell_size - 1) / cell_size;
    grid.resize(grid_w * grid_h);
}

void Particles::addSprite(const float pos_x, const float pos_y, const float vel_x, const float vel_y)
{
    x.push_back(pos_x);
    y.push_back(pos_y);
    vx.push_back(vel_x);
    vy.push_back(vel_y);
}

void Particles::clearSprites()
{
    x.clear();
    y.clear();
    vx.clear();
    vy.clear();
}

void Particles::doubleSprites(const size_t max_count)
{
    const size_t current = x.size();
    const size_t target = std::min(max_count, current * 2);

    for (size_t i = current; i < target; ++i)
    {
        const float angle = randFloat(0.0f, 2.0f * static_cast<float>(M_PI));
        constexpr float speed = 300.0f;
        addSprite(
            randFloat(0.0f, static_cast<float>(screen_width) - w),
            randFloat(0.0f, static_cast<float>(screen_height) - h),
            cosf(angle) * speed,
            sinf(angle) * speed
        );
    }
}

void Particles::halveSprites()
{
    const size_t current = x.size();
    if (current > 1)
    {
        const size_t new_size = current / 2;
        x.resize(new_size);
        y.resize(new_size);
        vx.resize(new_size);
        vy.resize(new_size);
    }
}

void Particles::update(const float dt)
{
    const size_t count = x.size();

    // move particles
    for (size_t i = 0; i < count; ++i)
    {
        x[i] += vx[i] * dt;
        y[i] += vy[i] * dt;
    }

    // screen bounds collision
    for (size_t i = 0; i < count; ++i)
    {
        if (x[i] <= 0.0f) // left
        {
            x[i] = 0.0f;
            vx[i] = fabsf(vx[i]);
        }
        if (y[i] <= 0.0f) // top
        {
            y[i] = 0.0f;
            vy[i] = fabsf(vy[i]);
        }
        if (x[i] + w >= static_cast<float>(screen_width)) // right
        {
            x[i] = static_cast<float>(screen_width) - w;
            vx[i] = -fabsf(vx[i]);
        }
        if (y[i] + h >= static_cast<float>(screen_height)) // bottom
        {
            y[i] = static_cast<float>(screen_height) - h;
            vy[i] = -fabsf(vy[i]);
        }
    }

    for (auto& cell : grid)
        cell.clear();

    // populate grid cells
    for (size_t i = 0; i < count; ++i)
    {
        const int gx = static_cast<int>(x[i]) / cell_size;
        const int gy = static_cast<int>(y[i]) / cell_size;
        // out of bounds check
        if (gx < 0 || gx >= grid_w || gy < 0 || gy >= grid_h)
            continue;
        const int index = gy * grid_w + gx;

        if (index >= 0 && index < static_cast<int>(grid.size()))
            grid[index].push_back(i);
    }

    for (auto& cell : grid)
    {
        constexpr size_t kCollisionCapPerCell = 32; // limit for performance
        if (cell.empty()) continue;

        // check collisions within cell
        const size_t localCount = std::min(cell.size(), kCollisionCapPerCell);
        for (size_t i = 0; i < localCount; ++i)
        {
            const size_t a = cell[i];

            for (size_t j = i + 1; j < localCount; ++j)
            {
                const size_t b = cell[j];
                // AABB collision check
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

void Particles::render(SDL_Renderer* renderer, SDL_Texture* texture) const
{
    const size_t count = x.size();
    for (size_t i = 0; i < count; ++i)
    {
        SDL_FRect dst{x[i], y[i], w, h};
        SDL_RenderTexture(renderer, texture, nullptr, &dst);
    }
}