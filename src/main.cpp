
//For understanding
/*
1. Fighter &target passes by reference, 
so the function changes the real fighter in the array. 
The other functions take Fighter f (a copy) because they only read it.

2. But it could also use Fighter (const Fighter &f) instaed Fighter f,
which would also avoid making a copy, save memory, but it would be read-only.

3. Getters: read-only access to private data

4. We use bubbleSort for the turn order at first

5. We use array for fighters but circular linked list is better for turn order, 
because we can just move the head pointer to the next fighter after each turn, 
instead of rebuilding the array every round. While The number of fighters is small and fixed. 
We are not going to add or remove fighters during the battle, so an array is fine.

6. TurnQueue() : tail(nullptr), current(nullptr), size(0) {} is Constructor write in shortcut of initializer list

7. cin.ignore(10000, '\n');
    - ignore() discards characters from the input buffer.
    - 10000 is the maximum number of characters to ignore.
    - '\n' is the delimiter, so it will stop ignoring when it sees a newline character.
    - This is used to clear out any extra input after reading a number, so that the next input starts fresh.

*/

#include "Battle.h"
#include "Fighter.h"
#include <cstdlib>   // srand()
#include <ctime>     // time()
 
int main() {
    srand(time(0));
 
    Battle battle;
    //                        name      HP  MP  ATK DEF SPD  hero
    battle.addFighter(Fighter("Knight", 120,  0, 18, 10,  8, true));
    battle.addFighter(Fighter("Mage",    80, 30, 22,  6, 12, true));
    battle.addFighter(Fighter("Slime",   60,  0, 12,  4,  6, false));
    battle.addFighter(Fighter("Goblin",  70,  0, 15,  6, 10, false));
    battle.addFighter(Fighter("Imp",     50, 10, 13,  3, 11, false));   // casts Fireball
 
    battle.run();
    return 0;
}
 