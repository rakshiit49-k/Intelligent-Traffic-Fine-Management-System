// =====================================================================
// Violation.h  —  MEMBER 2: RAKSHIT
// Violation & Fine Calculation Module
//
//   OOP side : Violation (abstract) + SpeedViolation, SignalJump,
//              NoHelmet, WrongParking  (inheritance + polymorphism)
//   DSA side : ViolationNode singly linked list (C-style struct +
//              pointers, no STL)
// =====================================================================
#ifndef VIOLATION_H
#define VIOLATION_H

#include <iostream>
#include <string>
using namespace std;

// ---------------------------------------------------------------
// OOP SIDE
// ---------------------------------------------------------------
class Violation {
protected:
    string date;      // "YYYY-MM-DD"
    int finalFine;    // base fine + repeat-offender surcharge (set when logged)
    bool paid;

public:
    Violation(string d) : date(d), finalFine(0), paid(false) {}

    virtual int calculateFine() = 0;     // pure virtual -> abstract class
    virtual string getName() = 0;
    virtual int getTypeCode() = 0;       // used by file save/load

    string getDate() { return date; }
    int getFinalFine() { return finalFine; }
    void setFinalFine(int f) { finalFine = f; }
    bool isPaid() { return paid; }
    void markPaid() { paid = true; }
    void setPaid(bool p) { paid = p; }

    virtual ~Violation() {}
};

class SpeedViolation : public Violation {
public:
    SpeedViolation(string d) : Violation(d) {}
    int calculateFine() override { return 500; }
    string getName() override { return "Speed Violation"; }
    int getTypeCode() override { return 1; }
};

class SignalJump : public Violation {
public:
    SignalJump(string d) : Violation(d) {}
    int calculateFine() override { return 1000; }
    string getName() override { return "Signal Jump"; }
    int getTypeCode() override { return 2; }
};

class NoHelmet : public Violation {
public:
    NoHelmet(string d) : Violation(d) {}
    int calculateFine() override { return 200; }
    string getName() override { return "No Helmet"; }
    int getTypeCode() override { return 3; }
};

class WrongParking : public Violation {
public:
    WrongParking(string d) : Violation(d) {}
    int calculateFine() override { return 300; }
    string getName() override { return "Wrong Parking"; }
    int getTypeCode() override { return 4; }
};

// Factory: builds the right violation object from a type number.
// Used by the menu (main.cpp) and by file loading (Payment.h).
Violation* createViolation(int type, string date) {
    switch (type) {
        case 1: return new SpeedViolation(date);
        case 2: return new SignalJump(date);
        case 3: return new NoHelmet(date);
        case 4: return new WrongParking(date);
        default: return nullptr;
    }
}

// ---------------------------------------------------------------
// DSA SIDE: C-style singly linked list
// ---------------------------------------------------------------
struct ViolationNode {
    Violation* data;
    ViolationNode* next;
};

// Insert at HEAD (newest first) - O(1)
void pushViolation(ViolationNode*& head, Violation* v) {
    ViolationNode* node = new ViolationNode();
    node->data = v;
    node->next = head;
    head = node;
}

// Insert at TAIL (keeps order when loading from file) - O(n)
void appendViolation(ViolationNode*& head, Violation* v) {
    ViolationNode* node = new ViolationNode();
    node->data = v;
    node->next = nullptr;
    if (head == nullptr) {
        head = node;
        return;
    }
    ViolationNode* cur = head;
    while (cur->next != nullptr) cur = cur->next;
    cur->next = node;
}

int countViolations(ViolationNode* head) {
    int count = 0;
    for (ViolationNode* cur = head; cur != nullptr; cur = cur->next) count++;
    return count;
}

// Total of all UNPAID fines
int totalDues(ViolationNode* head) {
    int sum = 0;
    for (ViolationNode* cur = head; cur != nullptr; cur = cur->next)
        if (!cur->data->isPaid()) sum += cur->data->getFinalFine();
    return sum;
}

// Repeat-offender rule: +20% of the base fine for EACH earlier violation.
// Call this BEFORE pushing the new violation into the list.
int computeFinalFine(ViolationNode* head, Violation* v) {
    int base = v->calculateFine();        // polymorphism: right formula runs
    int prior = countViolations(head);
    int surcharge = (base * 20 * prior) / 100;
    return base + surcharge;
}

void printHistory(ViolationNode* head) {
    if (head == nullptr) {
        cout << "  (no violations)" << endl;
        return;
    }
    int i = 1;
    for (ViolationNode* cur = head; cur != nullptr; cur = cur->next, i++) {
        cout << "  " << i << ". " << cur->data->getName()
             << " | " << cur->data->getDate()
             << " | Rs." << cur->data->getFinalFine()
             << " | " << (cur->data->isPaid() ? "PAID" : "UNPAID") << endl;
    }
}

// Deletes every node AND every violation object
void freeList(ViolationNode*& head) {
    while (head != nullptr) {
        ViolationNode* temp = head;
        head = head->next;
        delete temp->data;
        delete temp;
    }
}

#endif
