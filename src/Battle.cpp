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
 
// The living hero with the lowest HP (-1 if none)
int Battle::findWeakestHero() const {
    int weakest = -1;
    for (int i = 0; i < count; i++) {
        if (fighters[i].isHero() && fighters[i].isAlive()) {
            if (weakest == -1 || fighters[i].getHp() < fighters[weakest].getHp()) {
                weakest = i;
            }
        }
    }
    return weakest;
}
 
// ----------------------------------------------------------- turns ---
void Battle::takeTurn(int index) {
    fighters[index].setDefending(false);
 
    if (fighters[index].isHero()) {
        playerTurn(index);
    } else {
        enemyTurn(index);
    }
}
 
void Battle::playerTurn(int index) {
    Fighter &hero = fighters[index];
 
    cout << endl;
    printAllStats();
 
    // Loop until the player picks an action they can actually do.
    // Choosing a spell without enough MP shows the menu again.
    bool done = false;
    while (!done) {
        cout << "\n" << hero.getName() << "'s turn (MP " << hero.getMp() << "). Choose an action:" << endl;
        cout << "  1. Attack" << endl;
        cout << "  2. Defend" << endl;
        cout << "  3. Fireball (" << FIREBALL_COST << " MP)" << endl;
        cout << "  4. Heal     (" << HEAL_COST << " MP)" << endl;
 
        int choice = readChoice(1, 4);
 
        switch (choice) {
            case ACTION_ATTACK:
                performAttack(index, chooseTarget(false));
                done = true;
                break;
 
            case ACTION_DEFEND:
                performDefend(index);
                done = true;
                break;
 
            case ACTION_FIREBALL:
                if (hero.getMp() < FIREBALL_COST) {
                    cout << "Not enough MP!" << endl;
                } else {
                    performFireball(index, chooseTarget(false));
                    done = true;
                }
                break;
 
            case ACTION_HEAL:
                if (hero.getMp() < HEAL_COST) {
                    cout << "Not enough MP!" << endl;
                } else {
                    performHeal(index, chooseTarget(true));   // heal targets allies
                    done = true;
                }
                break;
        }
    }
}
 
// Smarter AI: decisions based on the situation, with some randomness
// so the player can't predict it perfectly.
void Battle::enemyTurn(int index) {
    Fighter &enemy = fighters[index];
 
    // 1. Badly hurt? Sometimes defend instead of attacking.
    bool lowHp = enemy.getHp() < enemy.getMaxHp() / 3;
    if (lowHp && randInt(1, 100) <= 40) {
        performDefend(index);
        return;
    }
 
    // 2. Usually go for the weakest hero, to try to knock one out.
    //    Otherwise pick a random hero.
    int target;
    if (randInt(1, 100) <= 60) {
        target = findWeakestHero();
    } else {
        int options[MAX_FIGHTERS];
        int numOptions = listTargets(true, options);
        target = options[randInt(0, numOptions - 1)];
    }
 
    // 3. Enemies with MP use Fireball when they can afford it.
    if (enemy.getMp() >= FIREBALL_COST && randInt(1, 100) <= 50) {
        performFireball(index, target);
    } else {
        performAttack(index, target);
    }
}
 
int Battle::chooseTarget(bool heroSide) const {
    int options[MAX_FIGHTERS];
    int numOptions = listTargets(heroSide, options);
 
    cout << "Choose a target:" << endl;
    for (int i = 0; i < numOptions; i++) {
        const Fighter &f = fighters[options[i]];
        cout << "  " << (i + 1) << ". " << f.getName()
             << " (HP " << f.getHp() << "/" << f.getMaxHp() << ")" << endl;
    }
 
    int choice = readChoice(1, numOptions);
    return options[choice - 1];
}
 
// --------------------------------------------------------- actions ---
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
 
    handleDefeat(targetIndex);
}
 
void Battle::performFireball(int casterIndex, int targetIndex) {
    Fighter &caster = fighters[casterIndex];
    Fighter &target = fighters[targetIndex];
 
    caster.spendMp(FIREBALL_COST);
 
    int dmg = caster.calcSpellDamage();
    if (target.isDefending()) {
        dmg = dmg / 2;
        if (dmg < 1) dmg = 1;
    }
    target.takeDamage(dmg);
 
    cout << caster.getName() << " casts Fireball on " << target.getName()
         << " for " << dmg << " damage"
         << (target.isDefending() ? " (defended)." : ".") << endl;
 
    handleDefeat(targetIndex);
}
 
void Battle::performHeal(int casterIndex, int targetIndex) {
    Fighter &caster = fighters[casterIndex];
    Fighter &target = fighters[targetIndex];
 
    caster.spendMp(HEAL_COST);
    int healed = target.heal(HEAL_AMOUNT);
 
    cout << caster.getName() << " heals " << target.getName()
         << " for " << healed << " HP." << endl;
}
 
void Battle::performDefend(int index) {
    fighters[index].setDefending(true);
    cout << fighters[index].getName() << " braces for impact." << endl;
}
 
// If the target just died, announce it and remove it from the turn order.
// Shared by every action that deals damage.
void Battle::handleDefeat(int targetIndex) {
    if (!fighters[targetIndex].isAlive()) {
        cout << fighters[targetIndex].getName() << " is defeated!" << endl;
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