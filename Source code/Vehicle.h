#ifndef VEHICLE_H
#define VEHICLE_H

#include <stdexcept>
#include <iomanip>
#include "Violation.h"
#include "Exceptions.h"

// OOP SIDE

class Owner {
private:
    string name;
    string phone;

public:
    Owner(string n = "", string p = "") : name(n), phone(p) {}
    string getName() const { return name; }
    string getPhone() const { return phone; }
};

class Vehicle {
protected:
    string regNumber;
    Owner owner;
    ViolationNode* violationHead;   // Rakshit's linked list lives inside each vehicle

public:
    Vehicle(string reg, Owner o) : regNumber(reg), owner(o), violationHead(nullptr) {}

    // a vehicle owns heap memory, so copying is not allowed
    Vehicle(const Vehicle&) = delete;
    Vehicle& operator=(const Vehicle&) = delete;

    virtual ~Vehicle() { freeList(violationHead); }

    virtual string getType() = 0;       
    virtual int getTypeCode() = 0;      

    string getRegNumber() { return regNumber; }
    string getOwnerName() { return owner.getName(); }
    string getOwnerPhone() { return owner.getPhone(); }
    ViolationNode* getViolationHead() { return violationHead; }

    // Log a NEW violation:
    void addViolation(Violation* v) {
        v->setFinalFine(computeFinalFine(violationHead, v));
        pushViolation(violationHead, v);
    }

    void restoreViolation(Violation* v) { appendViolation(violationHead, v); }

    int getViolationCount() { return countViolations(violationHead); }
    int getTotalDues() { return totalDues(violationHead); }

    void showDetails() {
        cout << "Vehicle  : " << regNumber << " (" << getType() << ")" << endl;
        cout << "Owner    : " << owner.getName() << " | " << owner.getPhone() << endl;
        cout << "Dues     : Rs." << getTotalDues() << endl;
        cout << "History  :" << endl;
        printHistory(violationHead);
    }
};

class TwoWheeler : public Vehicle {
public:
    TwoWheeler(string reg, Owner o) : Vehicle(reg, o) {}
    string getType() override { return "Two-Wheeler"; }
    int getTypeCode() override { return 1; }
};

class FourWheeler : public Vehicle {
public:
    FourWheeler(string reg, Owner o) : Vehicle(reg, o) {}
    string getType() override { return "Four-Wheeler"; }
    int getTypeCode() override { return 2; }
};

class CommercialVehicle : public Vehicle {
public:
    CommercialVehicle(string reg, Owner o) : Vehicle(reg, o) {}
    string getType() override { return "Commercial"; }
    int getTypeCode() override { return 3; }
};

// Factory: builds the right vehicle from a type number
Vehicle* createVehicle(int type, string reg, Owner o) {
    switch (type) {
        case 1: return new TwoWheeler(reg, o);
        case 2: return new FourWheeler(reg, o);
        case 3: return new CommercialVehicle(reg, o);
        default: return nullptr;
    }
}

// DSA SIDE: Hash table with separate chaining

struct HashNode {
    Vehicle* vehicle;
    HashNode* next;
};

class HashTable {
private:
    static const int SIZE = 101;    
    HashNode* buckets[SIZE];
    int count;

    // our own hash function:
    int hashFunction(const string& key) {
        unsigned long h = 0;
        for (size_t i = 0; i < key.length(); i++)
            h = h * 31 + (unsigned char)key[i];
        return (int)(h % SIZE);
    }

public:
    HashTable() : count(0) {
        for (int i = 0; i < SIZE; i++) buckets[i] = nullptr;
    }

    HashTable(const HashTable&) = delete;
    HashTable& operator=(const HashTable&) = delete;

    ~HashTable() {
        for (int i = 0; i < SIZE; i++) {
            HashNode* cur = buckets[i];
            while (cur != nullptr) {
                HashNode* temp = cur;
                cur = cur->next;
                delete temp->vehicle;
                delete temp;
            }
        }
    }

    bool insert(Vehicle* v) {
        if (search(v->getRegNumber()) != nullptr) return false;
        int idx = hashFunction(v->getRegNumber());
        HashNode* node = new HashNode();
        node->vehicle = v;
        node->next = buckets[idx];
        buckets[idx] = node;
        count++;
        return true;
    }

    Vehicle* search(const string& reg) {
        int idx = hashFunction(reg);
        for (HashNode* cur = buckets[idx]; cur != nullptr; cur = cur->next)
            if (cur->vehicle->getRegNumber() == reg) return cur->vehicle;
        return nullptr;
    }

    bool remove(const string& reg) {
        int idx = hashFunction(reg);
        HashNode* cur = buckets[idx];
        HashNode* prev = nullptr;
        while (cur != nullptr) {
            if (cur->vehicle->getRegNumber() == reg) {
                if (prev == nullptr) buckets[idx] = cur->next;
                else prev->next = cur->next;
                delete cur->vehicle;
                delete cur;
                count--;
                return true;
            }
            prev = cur;
            cur = cur->next;
        }
        return false;
    }

    int size() { return count; }

    int getAllVehicles(Vehicle* out[], int maxCount) {
        int n = 0;
        for (int i = 0; i < SIZE; i++)
            for (HashNode* cur = buckets[i]; cur != nullptr && n < maxCount; cur = cur->next)
                out[n++] = cur->vehicle;
        return n;
    }

    void display() {
        if (count == 0) {
            cout << "No vehicles registered." << endl;
            return;
        }
        cout << left << setw(7) << "Shelf" << setw(14) << "Vehicle" << setw(14) << "Type"
             << setw(14) << "Owner" << setw(12) << "Violations" << "Dues" << endl;
        int shelvesUsed = 0, longestChain = 0;
        for (int i = 0; i < SIZE; i++) {
            int chain = 0;
            for (HashNode* cur = buckets[i]; cur != nullptr; cur = cur->next) {
                Vehicle* v = cur->vehicle;
                cout << setw(7) << i << setw(14) << v->getRegNumber() << setw(14) << v->getType()
                     << setw(14) << v->getOwnerName() << setw(12) << v->getViolationCount()
                     << "Rs." << v->getTotalDues() << endl;
                chain++;
            }
            if (chain > 0) shelvesUsed++;
            if (chain > longestChain) longestChain = chain;
        }
        cout << right;
        cout << "Vehicles: " << count << " | Shelves used: " << shelvesUsed << " of " << SIZE
             << " | Longest chain: " << longestChain << endl;
    }
};


// Register a new vehicle
// Validates the number, rejects duplicates, builds the right subclass.

void registerVehicle(HashTable& table, int type, string reg, Owner owner) {
    reg = normalizeReg(reg);                               // may throw
    if (table.search(reg) != nullptr) throw DuplicateVehicleException();
    Vehicle* v = createVehicle(type, reg, owner);
    if (v == nullptr) throw invalid_argument("Unknown vehicle type.");
    table.insert(v);
    cout << "Vehicle " << reg << " registered (" << v->getType() << ")." << endl;
}

#endif
