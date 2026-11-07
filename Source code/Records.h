#ifndef RECORDS_H
#define RECORDS_H

#include "Vehicle.h"


// Record functions

// Search a vehicle by registration number 
Vehicle* findVehicle(HashTable& table, string reg) {
    Vehicle* v = table.search(toUpper(reg));
    if (v == nullptr) throw VehicleNotFoundException();
    return v;
}

// Log a violation against a vehicle
void logViolation(HashTable& table, string reg, Violation* v) {
    Vehicle* veh = table.search(toUpper(reg));
    if (veh == nullptr) {
        delete v;                             
        throw VehicleNotFoundException();
    }
    veh->addViolation(v);
    cout << v->getName() << " logged for " << veh->getRegNumber()
         << ". Fine: Rs." << v->getFinalFine();
    if (veh->getViolationCount() > 1)
        cout << " (includes repeat-offender surcharge)";
    cout << endl;
}

#endif
