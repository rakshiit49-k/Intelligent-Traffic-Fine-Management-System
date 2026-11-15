#ifndef VEHICLE_H
#define VEHICLE_H

#include <stdexcept>
#include <iomanip>
#include "Violation.h"
#include "Exceptions.h"

// OOP SIDE

class Owner 
{
private:
    string name;
    string phone;

public:
    Owner(string name = "", string phone = "") 
    {
        this->name = name;
        this->phone = phone;
    }
    
    string getName() const 
    { 
        return name; 
    }
    
    string getPhone() const 
    { 
        return phone; 
    }
};

class Vehicle 
{
protected:
    string regNumber;
    Owner owner;
    ViolationNode* violationHead;   // Voilation linked list lives inside each vehicle

public:
    Vehicle(string regNumber, Owner owner) 
    {
        this->regNumber = regNumber;
        this->owner = owner;
        this->violationHead = NULL;
    }

    // A vehicle owns heap memory, so copying is not allowed

    Vehicle(const Vehicle&) = delete;
    Vehicle& operator=(const Vehicle&) = delete;

    virtual ~Vehicle() 
    { 
        freeList(violationHead); 
    }

    virtual string getType() = 0;       
    virtual int getTypeCode() = 0;      

    string getRegNumber() 
    { 
        return regNumber; 
    }
    
    string getOwnerName() 
    { 
        return owner.getName(); 
    }
    
    string getOwnerPhone() 
    { 
        return owner.getPhone(); 
    }
    
    ViolationNode* getViolationHead() 
    { 
        return violationHead; 
    }

    // Log a NEW violation:
    void addViolation(Violation* violation) 
    {
        violation->setFinalFine(computeFinalFine(violationHead, violation));
        pushViolation(violationHead, violation);
    }

    void restoreViolation(Violation* violation) 
    { 
        appendViolation(violationHead, violation); 
    }

    int getViolationCount() 
    { 
        return countViolations(violationHead); 
    }
    
    int getTotalDues() 
    { 
        return totalDues(violationHead); 
    }

    void showDetails() 
    {
        cout << "Vehicle  : " << regNumber << " (" << getType() << ")" << endl;
        cout << "Owner    : " << owner.getName() << " | " << owner.getPhone() << endl;
        cout << "Dues     : Rs." << getTotalDues() << endl;
        cout << "History  :" << endl;
        printHistory(violationHead);
    }
};

class TwoWheeler : public Vehicle 
{
public:
    TwoWheeler(string regNumber, Owner owner) : Vehicle(regNumber, owner) {}
    
    string getType() override 
    { 
        return "Two-Wheeler"; 
    }
    
    int getTypeCode() override 
    { 
        return 1; 
    }
};

class FourWheeler : public Vehicle 
{
public:
    FourWheeler(string regNumber, Owner owner) : Vehicle(regNumber, owner) {}
    
    string getType() override 
    { 
        return "Four-Wheeler"; 
    }
    
    int getTypeCode() override 
    { 
        return 2; 
    }
};

class CommercialVehicle : public Vehicle 
{
public:
    CommercialVehicle(string regNumber, Owner owner) : Vehicle(regNumber, owner) {}
    
    string getType() override 
    { 
        return "Commercial"; 
    }
    
    int getTypeCode() override 
    { 
        return 3; 
    }
};

// Factory: builds the right vehicle from a type number
Vehicle* createVehicle(int type, string regNumber, Owner owner) 
{
    switch (type) 
    {
        case 1: 
            return new TwoWheeler(regNumber, owner);
        case 2: 
            return new FourWheeler(regNumber, owner);
        case 3: 
            return new CommercialVehicle(regNumber, owner);
        default: 
            return NULL;
    }
}

// DSA SIDE: Hash table with separate chaining

struct HashNode 
{
    Vehicle* vehicle;
    HashNode* next;
};

class HashTable 
{
private:
    static const int SIZE = 101;    
    HashNode* buckets[SIZE];
    int count;

    // Custom hash function:
    int hashFunction(const string& key) 
    {
        unsigned long h = 0;
        for (size_t i = 0; i < key.length(); i++)
        {
            h = h * 31 + (unsigned char)key[i];
        }
        return (int)(h % SIZE);
    }

public:
    HashTable() : count(0) 
    {
        for (int i = 0; i < SIZE; i++) 
        {
            buckets[i] = NULL;
        }
    }

    HashTable(const HashTable&) = delete;
    HashTable& operator=(const HashTable&) = delete;

    ~HashTable() 
    {
        for (int i = 0; i < SIZE; i++) 
        {
            HashNode* cur = buckets[i];
            while (cur != NULL) 
            {
                HashNode* temp = cur;
                cur = cur->next;
                delete temp->vehicle;
                delete temp;
            }
        }
    }

    bool insert(Vehicle* vehicle) 
    {
        if (search(vehicle->getRegNumber()) != NULL) 
        {
            return false;
        }
        
        int idx = hashFunction(vehicle->getRegNumber());
        HashNode* node = new HashNode();
        node->vehicle = vehicle;
        node->next = buckets[idx];
        buckets[idx] = node;
        count++;
        
        return true;
    }

    Vehicle* search(const string& regNumber) 
    {
        int idx = hashFunction(regNumber);
        for (HashNode* cur = buckets[idx]; cur != NULL; cur = cur->next)
        {
            if (cur->vehicle->getRegNumber() == regNumber) 
            {
                return cur->vehicle;
            }
        }
        return NULL;
    }

    bool remove(const string& regNumber) 
    {
        int idx = hashFunction(regNumber);
        HashNode* cur = buckets[idx];
        HashNode* prev = NULL;
        
        while (cur != NULL) 
        {
            if (cur->vehicle->getRegNumber() == regNumber) 
            {
                if (prev == NULL) 
                {
                    buckets[idx] = cur->next;
                }
                else 
                {
                    prev->next = cur->next;
                }
                
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

    int size() 
    { 
        return count; 
    }

    int getAllVehicles(Vehicle* out[], int maxCount) 
    {
        int n = 0;
        for (int i = 0; i < SIZE; i++)
        {
            for (HashNode* cur = buckets[i]; cur != NULL && n < maxCount; cur = cur->next)
            {
                out[n++] = cur->vehicle;
            }
        }
        return n;
    }

    void display() 
    {
        if (count == 0) 
        {
            cout << "No vehicles registered." << endl;
            return;
        }
        
        cout << left << setw(7) << "Shelf" << setw(14) << "Vehicle" << setw(14) << "Type"
             << setw(14) << "Owner" << setw(12) << "Violations" << "Dues" << endl;
             
        int shelvesUsed = 0;
        int longestChain = 0;
        
        for (int i = 0; i < SIZE; i++) 
        {
            int chain = 0;
            for (HashNode* cur = buckets[i]; cur != NULL; cur = cur->next) 
            {
                Vehicle* vehicle = cur->vehicle;
                cout << setw(7) << i << setw(14) << vehicle->getRegNumber() << setw(14) << vehicle->getType()
                     << setw(14) << vehicle->getOwnerName() << setw(12) << vehicle->getViolationCount()
                     << "Rs." << vehicle->getTotalDues() << endl;
                chain++;
            }
            if (chain > 0) 
            {
                shelvesUsed++;
            }
            if (chain > longestChain) 
            {
                longestChain = chain;
            }
        }
        
        cout << right;
        cout << "Vehicles: " << count << " | Shelves used: " << shelvesUsed << " of " << SIZE
             << " | Longest chain: " << longestChain << endl;
    }
};

// Register a new vehicle
// Validates the number, rejects duplicates, builds the right subclass.

void registerVehicle(HashTable& table, int type, string regNumber, Owner owner) 
{
    regNumber = normalizeReg(regNumber);                               
    
    if (table.search(regNumber) != NULL) 
    {
        throw DuplicateVehicleException();
    }
    
    Vehicle* vehicle = createVehicle(type, regNumber, owner);
    
    if (vehicle == NULL) 
    {
        throw invalid_argument("Unknown vehicle type.");
    }
    
    table.insert(vehicle);
    cout << "Vehicle " << regNumber << " registered (" << vehicle->getType() << ")." << endl;
}

#endif
