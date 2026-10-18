#include "GameUI.h"
using namespace std;

const int PANEL_Y = 420;           // where the bottom menu panel starts
const float ENEMY_DELAY = 0.8f;    // seconds before an enemy acts
const float FLASH_TIME  = 0.3f;    // seconds a fighter flashes when hit

const int MENU_COUNT = 4;
const string MENU_TEXT[MENU_COUNT] = { "Attack", "Defend", "Fireball (5 MP)", "Heal (4 MP)" };
// Menu row 0 = action 1, row 1 = action 2, ... (same numbers as the console menu)

// ---------------------------------------------------- setup / public ---
GameUI::GameUI() : menuIndex(0), targetIndex(0), enemyTimer(ENEMY_DELAY), logCount(0) {
    for (int i = 0; i < MAX_FIGHTERS; i++) {
        lastHp[i] = 0;
        flashTimer[i] = 0;
        sprites[i].id = 0;
    }
}

// Remember starting HP, and try to load "assets/<name>.png" for each fighter.
// If a file is missing, that fighter is drawn as a shape instead.
void GameUI::load(const Battle &battle) {
    for (int i = 0; i < battle.getFighterCount(); i++) {
        lastHp[i] = battle.getFighter(i).getHp();

        string path = "assets/" + battle.getFighter(i).getName() + ".png";
        if (FileExists(path.c_str())) {
            sprites[i] = LoadTexture(path.c_str());
        }
    }
}

void GameUI::unload(const Battle &battle) {
    for (int i = 0; i < battle.getFighterCount(); i++) {
        if (sprites[i].id != 0) {
            UnloadTexture(sprites[i]);
        }
    }
}

void GameUI::update(Battle &battle, float dt) {
    handleInput(battle);
    updateEnemyTurn(battle, dt);
    collectMessages(battle);
    updateFlashes(battle, dt);
}

void GameUI::draw(const Battle &battle) const {
    ClearBackground(BLACK);

    // Background: sky and ground
    DrawRectangleGradientV(0, 0, SCREEN_W, 200, { 40, 50, 90, 255 }, { 100, 90, 130, 255 });
    DrawRectangleGradientV(0, 200, SCREEN_W, PANEL_Y - 200, { 60, 90, 60, 255 }, { 30, 50, 30, 255 });

    for (int i = 0; i < battle.getFighterCount(); i++) {
        drawFighter(battle, i);
    }
    drawTargetCursor(battle);
    drawPanel(battle);
    drawEndScreen(battle);
}

// ----------------------------------------------------------- update ---
// Move Battle's new messages into our own log box (keep the newest few)
void GameUI::collectMessages(Battle &battle) {
    for (int i = 0; i < battle.getMessageCount(); i++) {
        if (logCount == LOG_LINES) {
            for (int j = 1; j < LOG_LINES; j++) {   // shift up, drop the oldest
                log[j - 1] = log[j];
            }
            logCount--;
        }
        log[logCount] = battle.getMessage(i);
        logCount++;
    }
    battle.clearMessages();
}

// If a fighter's HP dropped since last frame, start its flash
void GameUI::updateFlashes(const Battle &battle, float dt) {
    for (int i = 0; i < battle.getFighterCount(); i++) {
        int hp = battle.getFighter(i).getHp();
        if (hp < lastHp[i]) {
            flashTimer[i] = FLASH_TIME;
        }
        lastHp[i] = hp;

        if (flashTimer[i] > 0) {
            flashTimer[i] -= dt;
        }
    }
}

// Keyboard input. Same job as chooseAction()/chooseTarget() in the console,
// but it never waits: if no key was pressed this frame, it just returns.
void GameUI::handleInput(Battle &battle) {
    Phase phase = battle.getPhase();

    if (phase == PHASE_CHOOSE_ACTION) {
        if (IsKeyPressed(KEY_DOWN)) menuIndex = (menuIndex + 1) % MENU_COUNT;
        if (IsKeyPressed(KEY_UP))   menuIndex = (menuIndex + MENU_COUNT - 1) % MENU_COUNT;

        if (IsKeyPressed(KEY_ENTER)) {
            battle.selectAction(menuIndex + 1);   // row 0 -> action 1
            targetIndex = 0;
        }
    }
    else if (phase == PHASE_CHOOSE_TARGET) {
        int options[MAX_FIGHTERS];
        int numOptions = battle.getTargetOptions(options);

        if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_RIGHT)) {
            targetIndex = (targetIndex + 1) % numOptions;
        }
        if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_LEFT)) {
            targetIndex = (targetIndex + numOptions - 1) % numOptions;
        }

        if (IsKeyPressed(KEY_ENTER)) {
            battle.selectTarget(targetIndex);
            menuIndex = 0;
        }
        if (IsKeyPressed(KEY_BACKSPACE)) {
            battle.cancelTarget();
        }
    }
}

// Enemies wait a moment before acting, so the player can see what happens
void GameUI::updateEnemyTurn(Battle &battle, float dt) {
    if (battle.getPhase() != PHASE_ENEMY_TURN) {
        enemyTimer = ENEMY_DELAY;        // reset, ready for the next enemy
        return;
    }

    enemyTimer -= dt;
    if (enemyTimer <= 0) {
        battle.runEnemyTurn();
        enemyTimer = ENEMY_DELAY;
    }
}

// ------------------------------------------------------------- draw ---
// Heroes stand on the left, enemies on the right, spaced out by their
// position on their own side.
Vector2 GameUI::getFighterPosition(const Battle &battle, int index) const {
    bool hero = battle.getFighter(index).isHero();
    int slot = 0;
    for (int i = 0; i < index; i++) {
        if (battle.getFighter(i).isHero() == hero) slot++;
    }

    Vector2 pos;
    pos.x = hero ? 200.0f + slot * 60 : 640.0f + slot * 90;
    pos.y = 160.0f + slot * 90;
    return pos;
}

void GameUI::drawBar(int x, int y, int width, int value, int maxValue, Color color) const {
    DrawRectangle(x, y, width, 8, DARKGRAY);
    if (maxValue > 0) {
        DrawRectangle(x, y, width * value / maxValue, 8, color);
    }
}

void GameUI::drawFighter(const Battle &battle, int index) const {
    const Fighter &f = battle.getFighter(index);
    Vector2 pos = getFighterPosition(battle, index);

    // Choose a colour: grey if defeated, white while flashing from a hit
    Color color = f.isHero() ? SKYBLUE : ORANGE;
    if (!f.isAlive()) color = Fade(GRAY, 0.4f);
    else if (flashTimer[index] > 0) color = WHITE;

    if (sprites[index].id != 0) {
        // Draw the image, scaled so it's 80 pixels tall, centred on pos
        Texture2D tex = sprites[index];
        float scale = 80.0f / tex.height;
        Vector2 topLeft = { pos.x - tex.width * scale / 2, pos.y - 80 };
        Color tint = f.isAlive() ? WHITE : Fade(GRAY, 0.4f);
        if (flashTimer[index] > 0) tint = RED;
        DrawTextureEx(tex, topLeft, 0.0f, scale, tint);
    } else if (f.isHero()) {
        // No image: draw a simple person shape
        DrawCircle((int)pos.x, (int)pos.y - 62, 16, color);
        DrawRectangle((int)pos.x - 18, (int)pos.y - 46, 36, 46, color);
    } else {
        // No image: draw a blob monster
        DrawEllipse((int)pos.x, (int)pos.y - 30, 36, 30, color);
        DrawCircle((int)pos.x - 12, (int)pos.y - 38, 6, BLACK);
        DrawCircle((int)pos.x + 12, (int)pos.y - 38, 6, BLACK);
    }

    if (!f.isAlive()) return;

    // Name and HP bar under the fighter
    DrawText(f.getName().c_str(), (int)pos.x - 35, (int)pos.y + 6, 16, RAYWHITE);
    drawBar((int)pos.x - 35, (int)pos.y + 26, 70, f.getHp(), f.getMaxHp(), RED);

    if (f.isDefending()) {
        DrawCircleLines((int)pos.x, (int)pos.y - 40, 50, SKYBLUE);
    }

    // Gold arrow over whoever's turn it is
    bool waitingForAction = battle.getPhase() == PHASE_CHOOSE_ACTION ||
                            battle.getPhase() == PHASE_CHOOSE_TARGET ||
                            battle.getPhase() == PHASE_ENEMY_TURN;
    if (waitingForAction && battle.getCurrentFighterIndex() == index) {
        Vector2 a = { pos.x - 10, pos.y - 104 };
        Vector2 b = { pos.x,      pos.y - 90 };
        Vector2 c = { pos.x + 10, pos.y - 104 };
        DrawTriangle(a, b, c, GOLD);
    }
}

// Yellow box around the fighter the target cursor is on
void GameUI::drawTargetCursor(const Battle &battle) const {
    if (battle.getPhase() != PHASE_CHOOSE_TARGET) return;

    int options[MAX_FIGHTERS];
    int numOptions = battle.getTargetOptions(options);
    if (targetIndex >= numOptions) return;

    Vector2 pos = getFighterPosition(battle, options[targetIndex]);
    DrawRectangleLines((int)pos.x - 50, (int)pos.y - 88, 100, 124, YELLOW);
}

void GameUI::drawPanel(const Battle &battle) const {
    DrawRectangle(0, PANEL_Y, SCREEN_W, SCREEN_H - PANEL_Y, { 20, 20, 40, 255 });
    DrawRectangleLines(8, PANEL_Y + 8, SCREEN_W - 16, SCREEN_H - PANEL_Y - 16, RAYWHITE);

    // Left: hero HP and MP
    int y = PANEL_Y + 24;
    for (int i = 0; i < battle.getFighterCount(); i++) {
        const Fighter &f = battle.getFighter(i);
        if (!f.isHero()) continue;

        DrawText(TextFormat("%s  HP %d/%d", f.getName().c_str(), f.getHp(), f.getMaxHp()),
                 24, y, 18, f.isAlive() ? RAYWHITE : GRAY);
        drawBar(24, y + 24, 140, f.getHp(), f.getMaxHp(), RED);
        if (f.getMaxMp() > 0) {
            drawBar(174, y + 24, 80, f.getMp(), f.getMaxMp(), BLUE);
        }
        y += 56;
    }

    // Middle: the action menu (only on a hero's turn)
    Phase phase = battle.getPhase();
    if (phase == PHASE_CHOOSE_ACTION || phase == PHASE_CHOOSE_TARGET) {
        const Fighter &hero = battle.getFighter(battle.getCurrentFighterIndex());
        DrawText(TextFormat("%s's turn", hero.getName().c_str()), 320, PANEL_Y + 22, 18, GOLD);

        for (int i = 0; i < MENU_COUNT; i++) {
            bool selected = (i == menuIndex);
            DrawText(TextFormat("%s %s", selected ? ">" : " ", MENU_TEXT[i].c_str()),
                     320, PANEL_Y + 50 + i * 26, 20, selected ? YELLOW : RAYWHITE);
        }

        if (phase == PHASE_CHOOSE_TARGET) {
            DrawText("Arrows: pick target  Enter: confirm  Backspace: back",
                     24, SCREEN_H - 26, 14, LIGHTGRAY);
        } else {
            DrawText("Up/Down: choose  Enter: confirm", 24, SCREEN_H - 26, 14, LIGHTGRAY);
        }
    }

    // Right: the message log
    for (int i = 0; i < logCount; i++) {
        DrawText(log[i].c_str(), 580, PANEL_Y + 22 + i * 28, 16, LIGHTGRAY);
    }
}

void GameUI::drawEndScreen(const Battle &battle) const {
    Phase phase = battle.getPhase();
    if (phase != PHASE_VICTORY && phase != PHASE_DEFEAT) return;

    const char *text = (phase == PHASE_VICTORY) ? "VICTORY" : "DEFEAT";
    Color color = (phase == PHASE_VICTORY) ? GOLD : RED;

    DrawRectangle(0, 0, SCREEN_W, PANEL_Y, Fade(BLACK, 0.6f));
    DrawText(text, SCREEN_W / 2 - MeasureText(text, 60) / 2, 150, 60, color);
    DrawText("Press Esc to quit", SCREEN_W / 2 - MeasureText("Press Esc to quit", 20) / 2,
             230, 20, RAYWHITE);
}