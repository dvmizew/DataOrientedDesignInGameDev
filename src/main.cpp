#include <iostream>
#include <cmath>
#include "../lib/Engine.h"
#include "../lib/Particles.h"
#include "../lib/Entity.h"
#include "../lib/Utils.h"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define SPRITE_SIZE 32.0f
#define MAX_SPRITES 100000

enum class GameMode { MENU, SCREENSAVER, ECS_DEMO };

static void spawnRandom(Particles& m, const int count) {
    const float maxX = static_cast<float>(m.screen_width) - SPRITE_SIZE;
    const float maxY = static_cast<float>(m.screen_height) - SPRITE_SIZE;
    for (int i = 0; i < count; ++i) {
        const float angle = randFloat(0.0f, 2.0f * static_cast<float>(M_PI));
        constexpr float speed = 300.0f;
        m.addSprite(randFloat(0.0f, maxX), randFloat(0.0f, maxY),
                    std::cos(angle) * speed, std::sin(angle) * speed);
    }
}

class ParticleSim : public GameEngine {
private:
    GameMode currentMode;
    Particles manager;
    ECSWorld ecsWorld;

    EntityID paddle{};
    EntityID ball{};

    bool upPressed = false, downPressed = false;
    bool onePressed = false, twoPressed = false;

    float screenW() const { return static_cast<float>(getScreenWidth()); }
    float screenH() const { return static_cast<float>(getScreenHeight()); }

public:
    ParticleSim(const int w, const int h)
        : GameEngine(w, h, "DraganBall"),
          currentMode(GameMode::MENU),
          manager(w, h, 64, SPRITE_SIZE, SPRITE_SIZE) {}

    bool setup() { return initialize("../assets/fonts/Roboto.ttf", {"../assets/img/dragan.png"}); }

    void initScreensaver() { manager.clearSprites(); spawnRandom(manager, 1000); }

    void initECSDemo() {
        constexpr float pw = 100.0f, ph = 20.0f;
         paddle = createPaddle(ecsWorld, screenW() / 2.0f - pw / 2.0f, screenH() - ph - 20.0f, pw, ph);
         if (auto* p = ecsWorld.getComponent<Paddle>(paddle)) p->speed = 550.0f;

        const float bs = 20.0f; // larger balls
        constexpr int ballCount = 5;
        for (int i = 0; i < ballCount; ++i) {
            const float angle = randFloat(20.0f * static_cast<float>(M_PI) / 180.0f,
                                          60.0f * static_cast<float>(M_PI) / 180.0f);
            constexpr float speed = 500.0f;
            const float bx = screenW() / 2.0f - bs / 2.0f;
            const float by = screenH() / 2.0f - bs / 2.0f + i * 5.0f;
            const EntityID b = createBall(ecsWorld, bx, by,
                                          std::cos(angle) * speed, std::sin(angle) * speed, bs);
            if (auto* r = ecsWorld.getComponent<Renderable>(b)) r->textureID = 0;
        }
    }

protected:
    void onUpdate(const float dt) override {
        if (currentMode == GameMode::MENU) { updateMenu(); return; }
        if (currentMode == GameMode::SCREENSAVER) updateScreensaver(dt);
        else updateECSDemo(dt);
    }

    void updateMenu() {
        const InputState& inp = getInput();
        const bool oneNow = inp.keys.count(SDLK_1) && inp.keys.at(SDLK_1);
        const bool twoNow = inp.keys.count(SDLK_2) && inp.keys.at(SDLK_2);
        if (oneNow && !onePressed) { currentMode = GameMode::SCREENSAVER; initScreensaver(); }
        if (twoNow && !twoPressed) { currentMode = GameMode::ECS_DEMO; initECSDemo(); }
        onePressed = oneNow; twoPressed = twoNow;
    }

    void updateScreensaver(const float dt) {
        setMonitoredParticleCount(manager.getCount());
        const InputState& inp = getInput();
        const bool upNow = inp.keys.count(SDLK_UP) && inp.keys.at(SDLK_UP);
        const bool downNow = inp.keys.count(SDLK_DOWN) && inp.keys.at(SDLK_DOWN);
        if (upNow && !upPressed) { manager.doubleSprites(MAX_SPRITES); }
        if (downNow && !downPressed) { manager.halveSprites(); if (manager.getCount() == 0) spawnRandom(manager, 1); }
        upPressed = upNow; downPressed = downNow;
        manager.update(dt);
    }

    void updateECSDemo(const float dt) {
        // for performance monitor
        const size_t entityCount = ecsWorld.getEntitiesWith<Transform>().size();
        setMonitoredParticleCount(entityCount);

        // input to move paddle
        if (auto* pt = ecsWorld.getComponent<Transform>(paddle)) {
            float vx = 0.0f;
            if (auto* p = ecsWorld.getComponent<Paddle>(paddle)) {
                const InputState& inp = getInput();
                if (inp.keys.count(SDLK_A) && inp.keys.at(SDLK_A)) vx = -p->speed;
                if (inp.keys.count(SDLK_D) && inp.keys.at(SDLK_D)) vx = p->speed;
            }
            pt->vx = vx;
             // keep paddle in screen
             pt->x = std::max(0.0f, std::min(pt->x, screenW() - pt->w));
        }

        // move entities
        for (const auto ent : ecsWorld.getEntitiesWith<Transform>()) {
            if (auto* t = ecsWorld.getComponent<Transform>(ent)) {
                t->x += t->vx * dt;
                t->y += t->vy * dt;
                if (ent == paddle) {
                    t->x = std::max(0.0f, std::min(t->x, screenW() - t->w));
                }
            }
        }

        // ball bounds
        for (const auto ent : ecsWorld.getEntitiesWith<Ball>()) {
            if (auto* bt = ecsWorld.getComponent<Transform>(ent)) {
                // vertical bounds bounce
                if (bt->y <= 0.0f) { bt->y = 0.0f; bt->vy = std::fabs(bt->vy); }
                if (bt->y + bt->h >= screenH()) { bt->y = screenH() - bt->h; bt->vy = -std::fabs(bt->vy); }

                // horizontal bounds bounce
                if (bt->x <= 0.0f) { bt->x = 0.0f; bt->vx = std::fabs(bt->vx); }
                if (bt->x + bt->w >= screenW()) { bt->x = screenW() - bt->w; bt->vx = -std::fabs(bt->vx); }
            }
        }

        // Ball vs paddle AABB
        if (auto* pt = ecsWorld.getComponent<Transform>(paddle)) {
            for (const auto ent : ecsWorld.getEntitiesWith<Ball>()) {
                if (auto* bt = ecsWorld.getComponent<Transform>(ent)) {
                    const bool hit = pt->x < bt->x + bt->w && pt->x + pt->w > bt->x &&
                                     pt->y < bt->y + bt->h && pt->y + pt->h > bt->y;
                    if (hit) {
                        bt->y = pt->y - bt->h; // place ball above paddle
                        bt->vy = -std::fabs(bt->vy); // reflect upward
                        bt->vx += pt->vx * 0.25f; // spin for paddle motion
                    }
                }
            }
        }
    }

    void onRender() override {
        if (currentMode == GameMode::MENU) { renderMenu(); return; }
        SDL_Texture* const tex = getTexture(0); if (!tex) return;
        if (currentMode == GameMode::SCREENSAVER) manager.render(getRenderer(), tex);
        else renderECS(tex);
    }

    void renderMenu() const {
        SDL_SetRenderDrawColor(getRenderer(), 20, 20, 30, 255);
        SDL_RenderClear(getRenderer());
        if (SDL_Texture* bg = getTexture(0)) {
            const SDL_FRect dst{0.0f, 0.0f, screenW(), screenH()};
            SDL_RenderTexture(getRenderer(), bg, nullptr, &dst);
        }
        TTF_Font* font = getFont(); if (!font) return;
        constexpr SDL_Color white = {255, 255, 255, 255};
        constexpr SDL_Color yellow = {255, 255, 100, 255};
        const float cx = screenW() / 2.0f; const float cy = screenH() / 2.0f;
        renderText("DraganBall", static_cast<int>(cx), 100, white, true);
        renderText("Select Mode:", static_cast<int>(cx), 200, yellow, true);
        renderButton(cx, cy - 100.0f, "[1] SCREENSAVER", "UP/DOWN to add/remove particles", "",
                     50, 100, 200, 100, 150, 255, white);
        renderButton(cx, cy + 50.0f, "[2] DRAGANBALL PONG", "W/S to move paddle", "",
                     200, 50, 100, 255, 100, 150, white);
        renderText("Press 1 or 2 to select | ESC to quit",
                   static_cast<int>(cx), static_cast<int>(screenH()) - 50, yellow, true);
    }

    void renderButton(const float x, const float y, const char* title, const char* desc1, const char* desc2,
                      const int r1, const int g1, const int b1, const int r2, const int g2, const int b2, const SDL_Color color) const {
        const SDL_FRect btn = {x - 400.0f, y, 800.0f, 100.0f};
        SDL_SetRenderDrawColor(getRenderer(), static_cast<Uint8>(r1), static_cast<Uint8>(g1), static_cast<Uint8>(b1), 255);
        SDL_RenderFillRect(getRenderer(), &btn);
        SDL_SetRenderDrawColor(getRenderer(), static_cast<Uint8>(r2), static_cast<Uint8>(g2), static_cast<Uint8>(b2), 255);
        SDL_RenderRect(getRenderer(), &btn);
        renderText(title, static_cast<int>(x), static_cast<int>(y) + 35, color, true);
        renderText(desc1, static_cast<int>(x), static_cast<int>(y) + 70, color, true);
        renderText(desc2, static_cast<int>(x), static_cast<int>(y) + 105, color, true);
    }

    void renderText(const char* text, const int x, const int y, const SDL_Color color, const bool centered = false) const {
        TTF_Font* font = getFont(); if (!font) return;
        SDL_Surface* surface = TTF_RenderText_Solid(font, text, strlen(text), color); if (!surface) return;
        SDL_Texture* texture = SDL_CreateTextureFromSurface(getRenderer(), surface);
        if (texture) {
            SDL_FRect dst; dst.w = static_cast<float>(surface->w); dst.h = static_cast<float>(surface->h);
            dst.x = centered ? static_cast<float>(x) - dst.w / 2.0f : static_cast<float>(x);
            dst.y = static_cast<float>(y);
            SDL_RenderTexture(getRenderer(), texture, nullptr, &dst);
            SDL_DestroyTexture(texture);
        }
        SDL_DestroySurface(surface);
    }

    void renderECS(SDL_Texture* tex) {
        if (!tex) return;
        // paddle
        if (const auto* pt = ecsWorld.getComponent<Transform>(paddle)) {
            SDL_SetRenderDrawColor(getRenderer(), 255, 255, 255, 255);
            SDL_FRect r{pt->x, pt->y, pt->w, pt->h};
            SDL_RenderFillRect(getRenderer(), &r);
        }
        // balls
        for (const auto ent : ecsWorld.getEntitiesWith<Ball>()) {
            if (const auto* t = ecsWorld.getComponent<Transform>(ent)) {
                SDL_FRect dst{t->x, t->y, t->w, t->h};
                SDL_RenderTexture(getRenderer(), tex, nullptr, &dst);
            }
        }
    }
};

int main() {
    ParticleSim app(SCREEN_WIDTH, SCREEN_HEIGHT);
    if (!app.setup()) { std::cerr << "Setup failed\n"; return 1; }
    app.loop();
    return 0;
}