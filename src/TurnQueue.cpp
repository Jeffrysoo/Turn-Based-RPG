#include "TurnQueue.h"

TurnQueue::TurnQueue() : tail(nullptr), current(nullptr), size(0) {}

TurnQueue::~TurnQueue() {
    clear();
}

//add new fighter
void TurnQueue::add(int fighterIndex) {
    TurnNode *node = new TurnNode;
    node->fighterIndex = fighterIndex;

    if (tail == nullptr) {          // empty list
        node->next = node;          // a single node points to itself
        current = node;             // first fighter added goes first
    } else {
        node->next = tail->next;    // new node points to the first node
        tail->next = node;          // old last node points to new node
    }
    tail = node;
    size++;
}

// remove a fighter from the turn order
void TurnQueue::remove(int fighterIndex) {
    if (tail == nullptr) return;

    TurnNode *prev = tail;
    for (int i = 0; i < size; i++) {
        TurnNode *node = prev->next;

        if (node->fighterIndex == fighterIndex) {
            if (size == 1) {                 // removing the only node
                tail = nullptr;
                current = nullptr;
            } else {
                prev->next = node->next;     // skip over the node
                if (node == tail)    tail = prev;
                if (node == current) current = prev;  // advance() then lands on the right fighter
            }
            delete node;
            size--;
            return;
        }
        prev = node;
    }
}

// Moves to the next fighter's turn.
void TurnQueue::advance() {
    if (current != nullptr) {
        current = current->next;
    }
}

void TurnQueue::clear() {
    if (tail == nullptr) return;

    TurnNode *node = tail->next;    // start at the first node
    tail->next = nullptr;           // break the circle so the loop can end
    while (node != nullptr) {
        TurnNode *nextNode = node->next;
        delete node;
        node = nextNode;
    }
    tail = nullptr;
    current = nullptr;
    size = 0;
}

int TurnQueue::currentFighter() const { return current->fighterIndex; }
int TurnQueue::getSize() const        { return size; }
bool TurnQueue::isEmpty() const       { return size == 0; }

int TurnQueue::indexAt(int position) const {
    TurnNode *node = current;
    for (int i = 0; i < position; i++) {
        node = node->next;
    }
    return node->fighterIndex;
}