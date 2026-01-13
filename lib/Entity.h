#ifndef DATAORIENTEDDESIGNINGAMEDEV_ENTITY_H
#define DATAORIENTEDDESIGNINGAMEDEV_ENTITY_H

#include "ECS.h"

struct Transform : Component {
    float x, y;
    float vx, vy;
    float w, h;

    Transform(const float x = 0, const float y = 0, const float vx = 0, const float vy = 0, const float w = 32, const float h = 32)
        : x(x), y(y), vx(vx), vy(vy), w(w), h(h) {}
};

struct Renderable : Component {
    int textureID;
    bool visible;

    Renderable(const int texID = 0, const bool vis = true)
        : textureID(texID), visible(vis) {}
};

struct Player : Component {
    int health = 100;
    int score = 0;
    float speed = 400.0f;
};

struct Enemy : Component {
    int health = 10;
    float speed = 150.0f;
    int damage = 10;
};

struct Collectible : Component {
    int scoreValue = 10;
    bool collected = false;
};

struct Paddle : Component { float speed = 500.0f; };
struct Ball : Component { float speed = 500.0f; };
struct Wall : Component {};

inline EntityID createPlayer(ECSWorld& world, const float x, const float y) {
    const EntityID entity = world.createEntity();
    world.addComponent<Transform>(entity, Transform(x, y, 0, 0, 32, 32));
    world.addComponent<Renderable>(entity, Renderable(0, true));
    world.addComponent<Player>(entity, Player());
    return entity;
}

inline EntityID createEnemy(ECSWorld& world, const float x, const float y, const float vx, const float vy) {
    const EntityID entity = world.createEntity();
    world.addComponent<Transform>(entity, Transform(x, y, vx, vy, 32, 32));
    world.addComponent<Renderable>(entity, Renderable(0, true));
    world.addComponent<Enemy>(entity, Enemy());
    return entity;
}

inline EntityID createCollectible(ECSWorld& world, float x, float y) {
    const EntityID entity = world.createEntity();
    world.addComponent<Transform>(entity, Transform(x, y, 0, 0, 16, 16));
    world.addComponent<Renderable>(entity, Renderable(0, true));
    world.addComponent<Collectible>(entity, Collectible());
    return entity;
}

inline EntityID createPaddle(ECSWorld& world, float x, float y, float w, float h) {
    const EntityID e = world.createEntity();
    world.addComponent<Transform>(e, Transform(x, y, 0.0f, 0.0f, w, h));
    world.addComponent<Renderable>(e, Renderable(0, true));
    world.addComponent<Paddle>(e, Paddle());
    return e;
}

inline EntityID createBall(ECSWorld& world, float x, float y, float vx, float vy, float size) {
    const EntityID e = world.createEntity();
    world.addComponent<Transform>(e, Transform(x, y, vx, vy, size, size));
    world.addComponent<Renderable>(e, Renderable(0, true));
    world.addComponent<Ball>(e, Ball());
    return e;
}

inline EntityID createWall(ECSWorld& world, float x, float y, float w, float h) {
    const EntityID e = world.createEntity();
    world.addComponent<Transform>(e, Transform(x, y, 0.0f, 0.0f, w, h));
    world.addComponent<Renderable>(e, Renderable(0, true));
    world.addComponent<Wall>(e, Wall());
    return e;
}

#endif

