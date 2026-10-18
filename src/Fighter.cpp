#include "Fighter.h"
#include "Utils.h"
#include <iostream>
using namespace std;
 
Fighter::Fighter()
    : name(""), maxHp(1), hp(0), attack(0), defense(0), speed(0),
      hero(false), defending(false) {}
 
Fighter::Fighter(string n, int startHp, int atk, int def, int spd, bool isHeroUnit)
    : name(n), maxHp(startHp), hp(startHp), attack(atk),
      defense(def), speed(spd), hero(isHeroUnit), defending(false) {}
 
string Fighter::getName() const   { return name; }
int Fighter::getHp() const        { return hp; }
int Fighter::getSpeed() const     { return speed; }
bool Fighter::isHero() const      { return hero; }
bool Fighter::isAlive() const     { return hp > 0; }
bool Fighter::isDefending() const { return defending; }
void Fighter::setDefending(bool value) { defending = value; }
 
int Fighter::calcDamageAgainst(const Fighter &target) const {
    int base = attack - target.defense / 2;
    int damage = base * randInt(85, 115) / 100;
    if (damage < 1) damage = 1;
    return damage;
}
 
void Fighter::takeDamage(int amount) {
    hp -= amount;
    if (hp < 0) hp = 0;
}
 
void Fighter::printStats() const {
    cout << (hero ? "[Hero]  " : "[Enemy] ") << name
         << "  HP " << hp << "/" << maxHp
         << (defending ? "  (defending)" : "")
         << (isAlive() ? "" : "  (defeated)") << endl;
}
 