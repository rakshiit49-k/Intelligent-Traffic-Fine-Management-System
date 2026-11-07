// =====================================================================
// Records.h  —  MEMBER 3: DIVYANSHI
// Record Management Module
//
//   Functions : findVehicle  (search by reg. number, throws if missing)
//               logViolation (log a violation against a vehicle)
// =====================================================================
#ifndef RECORDS_H
#define RECORDS_H

#include "Vehicle.h"

// ---------------------------------------------------------------
// Record functions
// ---------------------------------------------------------------

// Search a vehicle by registration number (hash table lookup)
Vehicle* findVehicle(HashTable& table, string reg) {
    Vehicle* v = table.search(toUpper(reg));
    if (v == nullptr) throw VehicleNotFoundException();
    return v;
}

// Log a violation against a vehicle. The surcharge is applied inside
// Vehicle::addViolation (Rakshit's computeFinalFine).
void logViolation(HashTable& table, string reg, Violation* v) {
    Vehicle* veh = table.search(toUpper(reg));
    if (veh == nullptr) {
        delete v;                                  // avoid a memory leak
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
