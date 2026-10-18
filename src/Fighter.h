#ifndef FIGHTER_H
#define FIGHTER_H
 
#include <string>
 
class Fighter {
private:
    std::string name;
    int maxHp;
    int hp;
    int attack;
    int defense;
    int speed;
    bool hero;
    bool defending;   // NEW: true until this fighter's next turn
 
public:
    Fighter();
    Fighter(std::string n, int startHp, int atk, int def, int spd, bool isHeroUnit);
 
    std::string getName() const;
    int getHp() const;
    int getSpeed() const;
    bool isHero() const;
    bool isAlive() const;
    bool isDefending() const;         // NEW
    void setDefending(bool value);    // NEW
 
    // How much damage this fighter does to the target (doesn't change HP)
    int calcDamageAgainst(const Fighter &target) const;
 
    // The only way HP can go down
    void takeDamage(int amount);
 
    void printStats() const;
};
 
#endif
 