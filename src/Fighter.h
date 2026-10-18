#ifndef FIGHTER_H
#define FIGHTER_H
 
#include <string>
 
class Fighter {
private:
    std::string name;
    int maxHp;
    int hp;
    int maxMp;        // NEW
    int mp;           // NEW
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
    int getMaxHp() const;             // NEW: the AI needs this to judge "low HP"
    int getMp() const;                // NEW
    int getSpeed() const;
    bool isHero() const;
    bool isAlive() const;
    bool isDefending() const;
    void setDefending(bool value);
 
    // Physical attack: reduced by the target's defense
    int calcDamageAgainst(const Fighter &target) const;
 
    // NEW: magic attack: ignores defense, so it's good against tough enemies
    int calcSpellDamage() const;
 
    // NEW: spend MP. Returns false (and spends nothing) if there isn't enough.
    bool spendMp(int cost);
 
    void takeDamage(int amount);
 
    // NEW: restore HP, never above maxHp. Returns how much was actually healed.
    int heal(int amount);
 
    void printStats() const;
};
 
#endif