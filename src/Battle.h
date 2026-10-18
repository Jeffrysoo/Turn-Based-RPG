#ifndef BATTLE_H
#define BATTLE_H
 
#include "Fighter.h"
#include "TurnQueue.h"
 
const int MAX_FIGHTERS = 10;
 
// NEW: the actions a hero can choose from the menu
enum Action {
    ACTION_ATTACK = 1,
    ACTION_DEFEND = 2
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
 
    // fills options[] with indices of living fighters on one side,
    // returns how many it found. Shared by the player menu and the enemy AI.
    int listTargets(bool heroSide, int options[]) const;
 
    void takeTurn(int index);
    void playerTurn(int index);                    // NEW: hero, menu-driven
    void enemyTurn(int index);                     // NEW: enemy, automatic
    int chooseTarget(bool heroSide) const;         // NEW: player picks from a list
    void performAttack(int attackerIndex, int targetIndex);  // NEW: shared by both sides
 
public:
    Battle();
    void addFighter(const Fighter &f);
    void run();
};
 
#endif