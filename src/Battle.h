#ifndef BATTLE_H
#define BATTLE_H
 
#include "Fighter.h"
#include "TurnQueue.h"
 
const int MAX_FIGHTERS = 10;
 
// NEW: spell costs and power in one place, so balancing is easy
const int FIREBALL_COST = 5;
const int HEAL_COST     = 4;
const int HEAL_AMOUNT   = 30;
 
enum Action {
    ACTION_ATTACK   = 1,
    ACTION_DEFEND   = 2,
    ACTION_FIREBALL = 3,   // NEW
    ACTION_HEAL     = 4    // NEW
};
 
class Battle {
private:
    Fighter fighters[MAX_FIGHTERS];
    int count;
    TurnQueue turns;
 
    void buildTurnOrder();
    bool sideDefeated(bool heroSide) const;
    void printTurnOrder() const;
    void printAllStats() const;
    int listTargets(bool heroSide, int options[]) const;
 
    void takeTurn(int index);
    void playerTurn(int index);
    void enemyTurn(int index);
    int chooseTarget(bool heroSide) const;
 
    // Actions: shared by heroes and enemies
    void performAttack(int attackerIndex, int targetIndex);
    void performFireball(int casterIndex, int targetIndex);   // NEW
    void performHeal(int casterIndex, int targetIndex);       // NEW
    void performDefend(int index);                            // NEW
 
    // NEW: the AI's helpers
    int findWeakestHero() const;
    void handleDefeat(int targetIndex);
 
public:
    Battle();
    void addFighter(const Fighter &f);
    void run();
};
 
#endif