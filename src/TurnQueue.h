#ifndef TURNQUEUE_H
#define TURNQUEUE_H

// One node in the circular list: a fighter's index in Battle's array
struct TurnNode {
    int fighterIndex;
    TurnNode *next;
};

// Circular linked list of fighter indices.
// The last node points back to the first, so turns loop forever.
class TurnQueue {
private:
    TurnNode *tail;      // last node; tail->next is the first node
    TurnNode *current;   // whose turn it is right now
    int size;

public:
    TurnQueue();
    ~TurnQueue();

    // Forbid copying: two queues sharing nodes would delete them twice
    TurnQueue(const TurnQueue &) = delete;
    TurnQueue &operator=(const TurnQueue &) = delete;

    void add(int fighterIndex);       // add to the end of the order
    void remove(int fighterIndex);    // take a fighter out (e.g. defeated)
    void advance();                   // move to the next fighter
    void clear();                     // delete every node

    int currentFighter() const;
    int getSize() const;
    bool isEmpty() const;
    int indexAt(int position) const;  // walk from current without moving it
};

#endif
