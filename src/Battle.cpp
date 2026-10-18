#include "Battle.h"
#include "Utils.h"
#include <iostream>
using namespace std;
 
Battle::Battle() : count(0) {}
 
void Battle::addFighter(const Fighter &f) {
    if (count < MAX_FIGHTERS) {
        fighters[count] = f;
        count++;
    }
}
 
// ----------------------------------------------------------- setup ---
void Battle::buildTurnOrder() {
    bool added[MAX_FIGHTERS] = { false };
 
    for (int k = 0; k < count; k++) {
        int best = -1;
        for (int i = 0; i < count; i++) {
            if (!added[i] && (best == -1 ||
                fighters[i].getSpeed() > fighters[best].getSpeed())) {
                best = i;
            }
        }
        turns.add(best);
        added[best] = true;
    }
}
 
// --------------------------------------------------------- helpers ---
bool Battle::sideDefeated(bool heroSide) const {
    for (int i = 0; i < count; i++) {
        if (fighters[i].isHero() == heroSide && fighters[i].isAlive()) {
            return false;
        }
    }
    return true;
}
 
int Battle::listTargets(bool heroSide, int options[]) const {
    int numOptions = 0;
    for (int i = 0; i < count; i++) {
        if (fighters[i].isHero() == heroSide && fighters[i].isAlive()) {
            options[numOptions] = i;
            numOptions++;
        }
    }
    return numOptions;
}
 
void Battle::printTurnOrder() const {
    cout << "Turn order: ";
    for (int i = 0; i < turns.getSize(); i++) {
        cout << fighters[turns.indexAt(i)].getName() << " ";
    }
    cout << endl;
}
 
void Battle::printAllStats() const {
    for (int i = 0; i < count; i++) {
        fighters[i].printStats();
    }
}
 
// ----------------------------------------------------------- turns ---
void Battle::takeTurn(int index) {
    fighters[index].setDefending(false);   // Defend only lasts until your next turn
 
    if (fighters[index].isHero()) {
        playerTurn(index);
    } else {
        enemyTurn(index);
    }
}
 
void Battle::playerTurn(int index) {
    cout << endl;
    printAllStats();
    cout << "\n" << fighters[index].getName() << "'s turn. Choose an action:" << endl;
    cout << "  1. Attack" << endl;
    cout << "  2. Defend" << endl;
 
    int choice = readChoice(1, 2);
 
    switch (choice) {
        case ACTION_ATTACK: {
            int target = chooseTarget(false);   // heroes attack enemies
            performAttack(index, target);
            break;
        }
        case ACTION_DEFEND:
            fighters[index].setDefending(true);
            cout << fighters[index].getName() << " braces for impact." << endl;
            break;
    }
}
 
void Battle::enemyTurn(int index) {
    int options[MAX_FIGHTERS];
    int numOptions = listTargets(true, options);   // enemies attack heroes
    if (numOptions == 0) return;
 
    int target = options[randInt(0, numOptions - 1)];
    performAttack(index, target);
}
 
int Battle::chooseTarget(bool heroSide) const {
    int options[MAX_FIGHTERS];
    int numOptions = listTargets(heroSide, options);
 
    cout << "Choose a target:" << endl;
    for (int i = 0; i < numOptions; i++) {
        const Fighter &f = fighters[options[i]];
        cout << "  " << (i + 1) << ". " << f.getName()
             << " (HP " << f.getHp() << ")" << endl;
    }
 
    int choice = readChoice(1, numOptions);
    return options[choice - 1];   // menu starts at 1, arrays start at 0
}
 
void Battle::performAttack(int attackerIndex, int targetIndex) {
    Fighter &attacker = fighters[attackerIndex];
    Fighter &target   = fighters[targetIndex];
 
    int dmg = attacker.calcDamageAgainst(target);
    if (target.isDefending()) {
        dmg = dmg / 2;
        if (dmg < 1) dmg = 1;
    }
    target.takeDamage(dmg);
 
    cout << attacker.getName() << " attacks " << target.getName()
         << " for " << dmg << " damage"
         << (target.isDefending() ? " (defended)." : ".") << endl;
 
    if (!target.isAlive()) {
        cout << target.getName() << " is defeated!" << endl;
        turns.remove(targetIndex);
    }
}
 
// ------------------------------------------------------------- run ---
void Battle::run() {
    buildTurnOrder();
    printTurnOrder();
 
    while (!turns.isEmpty()) {
        takeTurn(turns.currentFighter());
 
        if (sideDefeated(false)) { cout << "\nVictory!" << endl; break; }
        if (sideDefeated(true))  { cout << "\nDefeat..." << endl; break; }
 
        turns.advance();
    }
 
    cout << endl;
    printAllStats();
}