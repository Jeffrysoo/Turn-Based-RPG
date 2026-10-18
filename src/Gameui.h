#ifndef GAMEUI_H
#define GAMEUI_H

#include "raylib.h"
#include "Battle.h"
#include <string>

const int SCREEN_W = 960;
const int SCREEN_H = 600;

// Everything about the screen: input, timers, drawing and sprites.
// Battle remembers the GAME; GameUI remembers the DISPLAY.
class GameUI {
private:
    static const int LOG_LINES = 5;

    int menuIndex;                    // which action the cursor is on
    int targetIndex;                  // which target the cursor is on
    float enemyTimer;                 // counts down before an enemy acts

    std::string log[LOG_LINES];       // last few messages, oldest first
    int logCount;

    int lastHp[MAX_FIGHTERS];         // HP last frame, to notice hits
    float flashTimer[MAX_FIGHTERS];   // > 0 while a fighter is flashing
    Texture2D sprites[MAX_FIGHTERS];  // optional images (id 0 = not loaded)

    // Update helpers
    void handleInput(Battle &battle);
    void updateEnemyTurn(Battle &battle, float dt);
    void collectMessages(Battle &battle);
    void updateFlashes(const Battle &battle, float dt);

    // Draw helpers
    Vector2 getFighterPosition(const Battle &battle, int index) const;
    void drawBar(int x, int y, int width, int value, int maxValue, Color color) const;
    void drawFighter(const Battle &battle, int index) const;
    void drawTargetCursor(const Battle &battle) const;
    void drawPanel(const Battle &battle) const;
    void drawEndScreen(const Battle &battle) const;

public:
    GameUI();
    void load(const Battle &battle);            // call AFTER InitWindow
    void unload(const Battle &battle);          // call BEFORE CloseWindow
    void update(Battle &battle, float dt);      // once per frame
    void draw(const Battle &battle) const;      // once per frame
};

#endif