#include "Battle.h"
#include "Utils.h"
using namespace std;

Battle::Battle()
    : count(0), phase(PHASE_CHOOSE_ACTION), pendingAction(ACTION_ATTACK),
      messageCount(0) {}

void Battle::addFighter(const Fighter &f) {
    if (count < MAX_FIGHTERS) {
        fighters[count] = f;
        count++;
    }
}

void Battle::start() {
    buildTurnOrder();
    addMessage("Monsters appear!");
    beginTurn();
}

// ------------------------------------------------------ turn flow ---
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

void Battle::beginTurn() {
    Fighter &current = fighters[turns.currentFighter()];
    current.setDefending(false);       // Defend lasts until your next turn

    if (current.isHero()) {
        phase = PHASE_CHOOSE_ACTION;
    } else {
        phase = PHASE_ENEMY_TURN;
    }
}

void Battle::endTurn() {
    if (sideDefeated(false)) { phase = PHASE_VICTORY; addMessage("Victory!"); return; }
    if (sideDefeated(true))  { phase = PHASE_DEFEAT;  addMessage("Your party has fallen..."); return; }

    turns.advance();
    beginTurn();
}

// ---------------------------------------------------------- input ---
bool Battle::selectAction(int action) {
    if (phase != PHASE_CHOOSE_ACTION) return false;   // ignore input at the wrong time

    const Fighter &hero = fighters[turns.currentFighter()];

    if (action == ACTION_FIREBALL && hero.getMp() < FIREBALL_COST) {
        addMessage("Not enough MP!");
        return false;
    }
    if (action == ACTION_HEAL && hero.getMp() < HEAL_COST) {
        addMessage("Not enough MP!");
        return false;
    }

    if (action == ACTION_DEFEND) {             // no target needed: act right away
        performDefend(turns.currentFighter());
        endTurn();
    } else {                                   // remember it, then ask for a target
        pendingAction = action;
        phase = PHASE_CHOOSE_TARGET;
    }
    return true;
}

void Battle::selectTarget(int optionNumber) {
    if (phase != PHASE_CHOOSE_TARGET) return;

    int options[MAX_FIGHTERS];
    int numOptions = getTargetOptions(options);
    if (optionNumber < 0 || optionNumber >= numOptions) return;

    int actor  = turns.currentFighter();
    int target = options[optionNumber];

    switch (pendingAction) {
        case ACTION_ATTACK:   performAttack(actor, target);   break;
        case ACTION_FIREBALL: performFireball(actor, target); break;
        case ACTION_HEAL:     performHeal(actor, target);     break;
        default: break;
    }
    endTurn();
}

void Battle::cancelTarget() {
    if (phase == PHASE_CHOOSE_TARGET) {
        phase = PHASE_CHOOSE_ACTION;
    }
}

// Same AI as Step 5, now triggered by the UI instead of a loop
void Battle::runEnemyTurn() {
    if (phase != PHASE_ENEMY_TURN) return;

    int index = turns.currentFighter();
    Fighter &enemy = fighters[index];

    bool lowHp = enemy.getHp() < enemy.getMaxHp() / 3;
    if (lowHp && randInt(1, 100) <= 40) {
        performDefend(index);
        endTurn();
        return;
    }

    int target;
    if (randInt(1, 100) <= 60) {
        target = findWeakestHero();
    } else {
        int options[MAX_FIGHTERS];
        int numOptions = listTargets(true, options);
        target = options[randInt(0, numOptions - 1)];
    }

    if (enemy.getMp() >= FIREBALL_COST && randInt(1, 100) <= 50) {
        performFireball(index, target);
    } else {
        performAttack(index, target);
    }
    endTurn();
}

// --------------------------------------------------------- output ---
Phase Battle::getPhase() const                     { return phase; }
int Battle::getFighterCount() const                { return count; }
const Fighter &Battle::getFighter(int index) const { return fighters[index]; }
int Battle::getCurrentFighterIndex() const         { return turns.currentFighter(); }
int Battle::getMessageCount() const                { return messageCount; }
string Battle::getMessage(int index) const         { return messages[index]; }
void Battle::clearMessages()                       { messageCount = 0; }

// Heal targets allies; everything else targets enemies
int Battle::getTargetOptions(int options[]) const {
    bool targetHeroes = (pendingAction == ACTION_HEAL);
    return listTargets(targetHeroes, options);
}

// -------------------------------------------------------- helpers ---
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

// Store a message for the UI. If the list is somehow full, ignore extras.
void Battle::addMessage(string message) {
    if (messageCount < MAX_MESSAGES) {
        messages[messageCount] = message;
        messageCount++;
    }
}

void Battle::handleDefeat(int targetIndex) {
    if (!fighters[targetIndex].isAlive()) {
        addMessage(fighters[targetIndex].getName() + " is defeated!");
        turns.remove(targetIndex);
    }
}

// -------------------------------------------------------- actions ---
// Same logic as Step 5; cout replaced by addLog
void Battle::performAttack(int attackerIndex, int targetIndex) {
    Fighter &attacker = fighters[attackerIndex];
    Fighter &target   = fighters[targetIndex];

    int dmg = attacker.calcDamageAgainst(target);
    if (target.isDefending()) {
        dmg = dmg / 2;
        if (dmg < 1) dmg = 1;
    }
    target.takeDamage(dmg);

    addMessage(attacker.getName() + " attacks " + target.getName() + " for "
           + to_string(dmg) + " damage" + (target.isDefending() ? " (defended)." : "."));
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

    addMessage(caster.getName() + " casts Fireball on " + target.getName() + " for "
           + to_string(dmg) + " damage" + (target.isDefending() ? " (defended)." : "."));
    handleDefeat(targetIndex);
}

void Battle::performHeal(int casterIndex, int targetIndex) {
    Fighter &caster = fighters[casterIndex];
    Fighter &target = fighters[targetIndex];

    caster.spendMp(HEAL_COST);
    int healed = target.heal(HEAL_AMOUNT);

    addMessage(caster.getName() + " heals " + target.getName() + " for "
           + to_string(healed) + " HP.");
}

void Battle::performDefend(int index) {
    fighters[index].setDefending(true);
    addMessage(fighters[index].getName() + " braces for impact.");
}