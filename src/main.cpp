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

class Game : public GameEngine {
private:
    GameMode currentMode; // Menu, Screensaver, ECS Pong game
    Particles manager;
    ECSWorld ecsWorld;

    EntityID paddle{};
    EntityID ball{};

    bool upPressed = false, downPressed = false;
    bool onePressed = false, twoPressed = false;

    float screenW() const { return static_cast<float>(getScreenWidth()); }
    float screenH() const { return static_cast<float>(getScreenHeight()); }

public:
    Game(const int w, const int h)
        : GameEngine(w, h, "DraganBall"),
          currentMode(GameMode::MENU),
          manager(w, h, 64, SPRITE_SIZE, SPRITE_SIZE) {}

    bool setup() { return initialize("../assets/fonts/Roboto.ttf", {"../assets/img/dragan.png"}); }

    void initScreensaver() { manager.clearSprites(); spawnRandom(manager, 1000); }

    void initECSGame() {
        constexpr float pw = 100.0f, ph = 20.0f; // paddle width/height
         paddle = createPaddle(ecsWorld, screenW() / 2.0f - pw / 2.0f, screenH() - ph - 20.0f, pw, ph); // centered at bottom
         if (auto* p = ecsWorld.getComponent<Paddle>(paddle)) p->speed = 550.0f;

        const float bs = 20.0f; // ball size
        constexpr int ballCount = 5;
        for (int i = 0; i < ballCount; ++i) {
            // random angle between 20 and 60 degrees
            const float angle = randFloat(20.0f * static_cast<float>(M_PI) / 180.0f,
                                          60.0f * static_cast<float>(M_PI) / 180.0f);
            constexpr float speed = 500.0f;
            // spawn balls in a vertical line in the center
            const float bx = screenW() / 2.0f - bs / 2.0f; // centered horizontally
            const float by = screenH() / 2.0f - bs / 2.0f + static_cast<float>(i) * 5.0f;
            createBall(ecsWorld, bx, by, std::cos(angle) * speed, std::sin(angle) * speed, bs);
        }
    }

protected:
    void onUpdate(const float dt) override {
        if (currentMode == GameMode::MENU) { updateMenu(); return; }
        if (currentMode == GameMode::SCREENSAVER) updateScreensaver(dt);
        else updateECSGame(dt);
    }

    void updateMenu() {
        const InputState& input = getInput();
        const bool keyOne = input.keys.count(SDLK_1) && input.keys.at(SDLK_1);
        const bool keyTwo = input.keys.count(SDLK_2) && input.keys.at(SDLK_2);
        if (keyOne && !onePressed) { currentMode = GameMode::SCREENSAVER; initScreensaver(); }
        if (keyTwo && !twoPressed) { currentMode = GameMode::ECS_DEMO; initECSGame(); }
        onePressed = keyOne; twoPressed = keyTwo;
    }

    void updateScreensaver(const float dt) {
        setMonitoredParticleCount(manager.getCount());
        const InputState& input = getInput();
        const bool keyUp = input.keys.count(SDLK_UP) && input.keys.at(SDLK_UP); // double particles
        const bool keyDown = input.keys.count(SDLK_DOWN) && input.keys.at(SDLK_DOWN); // halve particles
        if (keyUp && !upPressed) { manager.doubleSprites(MAX_SPRITES); }
        if (keyDown && !downPressed) { manager.halveSprites(); if (manager.getCount() == 0) spawnRandom(manager, 1); }
        upPressed = keyUp; downPressed = keyDown;
        manager.update(dt);
    }

    void updateECSGame(const float dt) {
        // for performance monitor
        const size_t entityCount = ecsWorld.getEntitiesWith<Transform>().size();
        setMonitoredParticleCount(entityCount);

        // input to move paddle
        if (auto* pt = ecsWorld.getComponent<Transform>(paddle)) {
            float vx = 0.0f;
            if (const auto* p = ecsWorld.getComponent<Paddle>(paddle)) {
                const InputState& input = getInput();
                if (input.keys.count(SDLK_A) && input.keys.at(SDLK_A)) vx = -p->speed;
                if (input.keys.count(SDLK_D) && input.keys.at(SDLK_D)) vx = p->speed;
            }
            pt->vx = vx; // set horizontal velocity
        }

        // move entities based on velocity
        for (const auto entity : ecsWorld.getEntitiesWith<Transform>()) {
            if (auto* t = ecsWorld.getComponent<Transform>(entity)) {
                t->x += t->vx * dt; // horizontal movement
                t->y += t->vy * dt; // vertical movement
                if (entity == paddle) {
                    t->x = std::max(0.0f, std::min(t->x, screenW() - t->w)); // make sure paddle stays on screen
                }
            }
        }

        // ball bounds
        for (const auto ent : ecsWorld.getEntitiesWith<Ball>()) {
            if (auto* bt = ecsWorld.getComponent<Transform>(ent)) {
                // vertical bounds bounce
                if (bt->y <= 0.0f) { bt->y = 0.0f; bt->vy = std::fabs(bt->vy); } // top
                if (bt->y + bt->h >= screenH()) { bt->y = screenH() - bt->h; bt->vy = -std::fabs(bt->vy); } // bottom

                // horizontal bounds bounce
                if (bt->x <= 0.0f) { bt->x = 0.0f; bt->vx = std::fabs(bt->vx); } // left
                if (bt->x + bt->w >= screenW()) { bt->x = screenW() - bt->w; bt->vx = -std::fabs(bt->vx); } // right
            }
        }

        // Ball vs paddle AABB
        if (const auto* pt = ecsWorld.getComponent<Transform>(paddle)) {
            for (const auto ent : ecsWorld.getEntitiesWith<Ball>()) {
                if (auto* bt = ecsWorld.getComponent<Transform>(ent)) {
                    const bool hit = pt->x < bt->x + bt->w && pt->x + pt->w > bt->x &&
                                     pt->y < bt->y + bt->h && pt->y + pt->h > bt->y; // AABB check (axis-aligned bounding box)
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
        SDL_Texture* const tex = getTexture(0);
        if (!tex) return;
        if (currentMode == GameMode::SCREENSAVER)
            manager.render(getRenderer(), tex);
        else renderECS(tex);
    }

    void renderMenu() const {
        SDL_SetRenderDrawColor(getRenderer(), 20, 20, 30, 255); // dark background
        SDL_RenderClear(getRenderer());
        if (SDL_Texture* bg = getTexture(0)) { // poza cu Dragan
            const SDL_FRect dst{0.0f, 0.0f, screenW(), screenH()}; // full screen
            SDL_RenderTexture(getRenderer(), bg, nullptr, &dst); // render background
        }
        TTF_Font* font = getFont();
        if (!font) return;
        constexpr SDL_Color white = {255, 255, 255, 255};
        constexpr SDL_Color yellow = {255, 255, 100, 255};
        // center positions
        const float cx = screenW() / 2.0f;
        const float cy = screenH() / 2.0f;
        renderText("DraganBall", static_cast<int>(cx), 100, white, true);
        renderText("Select Mode:", static_cast<int>(cx), 200, yellow, true);
        renderButton(cx, cy - 100.0f, "[1] SCREENSAVER", "UP/DOWN to add/remove particles", "",
                     50, 100, 200, 100, 150, 255, white);
        renderButton(cx, cy + 50.0f, "[2] DRAGANBALL PONG", "W/S to move paddle", "",
                     200, 50, 100, 255, 100, 150, white);
        renderText("Press 1 or 2 to select | ESC to quit",
                   static_cast<int>(cx), static_cast<int>(screenH()) - 50, yellow, true);
    }

    void renderButton(const float centerX, const float centerY, const char* mainText, const char* subText1, const char* subText2,
                      const int fillRed, const int fillGreen, const int fillBlue, const int borderRed, const int borderGreen, const int borderBlue, const SDL_Color textColor) const {
        const SDL_FRect btn = {centerX - 400.0f, centerY, 800.0f, 100.0f}; // button rectangle (800px wide, centered at centerX)
        // fill color
        SDL_SetRenderDrawColor(getRenderer(), static_cast<Uint8>(fillRed), static_cast<Uint8>(fillGreen), static_cast<Uint8>(fillBlue), 255);
        SDL_RenderFillRect(getRenderer(), &btn); // fill button background
        // border color
        SDL_SetRenderDrawColor(getRenderer(), static_cast<Uint8>(borderRed), static_cast<Uint8>(borderGreen), static_cast<Uint8>(borderBlue), 255);
        SDL_RenderRect(getRenderer(), &btn); // draw button border
        renderText(mainText, static_cast<int>(centerX), static_cast<int>(centerY) + 35, textColor, true);
        renderText(subText1, static_cast<int>(centerX), static_cast<int>(centerY) + 70, textColor, true);
        renderText(subText2, static_cast<int>(centerX), static_cast<int>(centerY) + 105, textColor, true);
    }

    void renderText(const char* text, const int x, const int y, const SDL_Color color, const bool centered = false) const {
        TTF_Font* font = getFont();
        if (!font) return;
        SDL_Surface* surface = TTF_RenderText_Solid(font, text, strlen(text), color);
        if (!surface) return;
        SDL_Texture* texture = SDL_CreateTextureFromSurface(getRenderer(), surface);
        if (texture) {
            SDL_FRect dst;
            dst.w = static_cast<float>(surface->w); // width
            dst.h = static_cast<float>(surface->h); // height
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
            SDL_SetRenderDrawColor(getRenderer(), 255, 255, 255, 255); // white paddle
            const SDL_FRect r{pt->x, pt->y, pt->w, pt->h};
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
    Game app(SCREEN_WIDTH, SCREEN_HEIGHT);
    if (!app.setup()) { std::cerr << "Setup failed\n"; return 1; }
    app.loop();
    return 0;
}