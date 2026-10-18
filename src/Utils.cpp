#include "Utils.h"
#include <cstdlib>   // rand()
#include <iostream>
using namespace std;
 
int randInt(int min, int max) {
    return rand() % (max - min + 1) + min;
}
 
int readChoice(int min, int max) {
    int choice;
    while (true) {
        cout << "> ";
        cin >> choice;
 
        if (cin.fail()) {               // they typed letters, not a number
            cin.clear();                // reset cin's error state
            cin.ignore(10000, '\n');    // throw away the bad input
            cout << "Please enter a number." << endl;
        } else if (choice < min || choice > max) {
            cout << "Please enter " << min << " to " << max << "." << endl;
        } else {
            cin.ignore(10000, '\n');    // clear anything extra on the line
            return choice;
        }
    }
}
 