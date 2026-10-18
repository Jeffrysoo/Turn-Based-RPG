#ifndef FIGHTER_H
#define FIGHTER_H

#include <string>

// Pure game data and rules. No printing: displaying is the UI's job.
class Fighter {
private:
    std::string name;
    int maxHp;
    int hp;
    int maxMp;
    int mp;
    int attack;
    int defense;
    int speed;
    bool hero;
    bool defending;

public:
    Fighter();
    Fighter(std::string n, int startHp, int startMp, int atk, int def, int spd, bool isHeroUnit);

    std::string getName() const;
    int getHp() const;
    int getMaxHp() const;
    int getMp() const;
    int getMaxMp() const;             // NEW: the UI needs it to show "MP 25/30"
    int getSpeed() const;
    bool isHero() const;
    bool isAlive() const;
    bool isDefending() const;
    void setDefending(bool value);

    int calcDamageAgainst(const Fighter &target) const;
    int calcSpellDamage() const;
    bool spendMp(int cost);
    void takeDamage(int amount);
    int heal(int amount);
};

#endif