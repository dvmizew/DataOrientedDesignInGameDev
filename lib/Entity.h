#ifndef DATAORIENTEDDESIGNINGAMEDEV_ENTITY_H
#define DATAORIENTEDDESIGNINGAMEDEV_ENTITY_H

#include "ECS.h"

struct Transform : Component {
    float x, y; // position
    float vx, vy; // velocity
    float w, h; // width and height

    Transform(const float x = 0, const float y = 0, const float vx = 0, const float vy = 0, const float w = 32, const float h = 32)
        : x(x), y(y), vx(vx), vy(vy), w(w), h(h) {}
};

struct Renderable : Component {
    int textureID;

    explicit Renderable(const int texID = 0)
        : textureID(texID) {}
};

struct Paddle : Component { float speed = 500.0f; };
struct Ball : Component { float speed = 500.0f; };

inline EntityID createPaddle(ECSWorld& world, const float x, const float y, const float w, const float h) {
    const EntityID e = world.createEntity();
    world.addComponent<Transform>(e, Transform(x, y, 0.0f, 0.0f, w, h));
    world.addComponent<Renderable>(e, Renderable(0));
    world.addComponent<Paddle>(e, Paddle());
    return e;
}

inline EntityID createBall(ECSWorld& world, const float x, const float y, const float vx, const float vy, const float size) {
    const EntityID e = world.createEntity();
    world.addComponent<Transform>(e, Transform(x, y, vx, vy, size, size));
    world.addComponent<Renderable>(e, Renderable(0)); // texture ID 0 = dragan.png
    world.addComponent<Ball>(e, Ball());
    return e;
}

#endif