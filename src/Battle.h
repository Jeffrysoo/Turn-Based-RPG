#ifndef BATTLE_H
#define BATTLE_H

#include <string>
#include "Fighter.h"
#include "TurnQueue.h"

const int MAX_FIGHTERS = 10;
const int MAX_MESSAGES = 20;    // messages waiting for the UI to show them

const int FIREBALL_COST = 5;
const int HEAL_COST     = 4;
const int HEAL_AMOUNT   = 30;

enum Action {
    ACTION_ATTACK   = 1,
    ACTION_DEFEND   = 2,
    ACTION_FIREBALL = 3,
    ACTION_HEAL     = 4
};

// NEW: the battle is always in exactly one of these phases.
// The UI looks at the phase to decide what to show and what input to accept.
enum Phase {
    PHASE_CHOOSE_ACTION,   // waiting for the player to pick an action
    PHASE_CHOOSE_TARGET,   // waiting for the player to pick a target
    PHASE_ENEMY_TURN,      // an enemy is about to act
    PHASE_VICTORY,
    PHASE_DEFEAT
};

// The battle rules. It never reads input or prints anything.
// A UI (console now, raylib later) calls these functions and displays the result.
class Battle {
private:
    Fighter fighters[MAX_FIGHTERS];
    int count;
    TurnQueue turns;

    Phase phase;
    int pendingAction;             // the action waiting for a target

    std::string messages[MAX_MESSAGES];   // messages the UI hasn't shown yet
    int messageCount;

    // Setup and turn flow
    void buildTurnOrder();
    void beginTurn();              // set the phase for whoever acts now
    void endTurn();                // check win/lose, then move to the next fighter

    // Helpers
    bool sideDefeated(bool heroSide) const;
    int listTargets(bool heroSide, int options[]) const;
    int findWeakestHero() const;
    void addMessage(std::string message);
    void handleDefeat(int targetIndex);

    // Actions (shared by heroes and enemies)
    void performAttack(int attackerIndex, int targetIndex);
    void performFireball(int casterIndex, int targetIndex);
    void performHeal(int casterIndex, int targetIndex);
    void performDefend(int index);

public:
    Battle();
    void addFighter(const Fighter &f);
    void start();

    // ---- Input: the UI calls these when the player does something ----
    bool selectAction(int action);      // false if not allowed (e.g. no MP)
    void selectTarget(int optionNumber);// 0-based position in getTargetOptions()
    void cancelTarget();                // go back to the action menu
    void runEnemyTurn();                // the UI decides WHEN the enemy acts

    // ---- Output: the UI reads these to draw the screen ----
    Phase getPhase() const;
    int getFighterCount() const;
    const Fighter &getFighter(int index) const;
    int getCurrentFighterIndex() const;
    int getTargetOptions(int options[]) const;
    int getMessageCount() const;
    std::string getMessage(int index) const;
    void clearMessages();               // the UI calls this after showing them
};

#endif