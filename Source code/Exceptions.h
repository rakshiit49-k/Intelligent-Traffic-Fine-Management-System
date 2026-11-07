// =====================================================================
// Exceptions.h  —  MEMBER 3: DIVYANSHI
// Custom exception classes + input validation helpers
//
// No dependencies, so every other header can include it safely.
// =====================================================================
#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <exception>
#include <string>
#include <cctype>
using namespace std;

class InvalidRegNumberException : public exception {
public:
    const char* what() const noexcept override {
        return "Invalid vehicle number (use 6-10 letters/digits, no spaces).";
    }
};

class DuplicateVehicleException : public exception {
public:
    const char* what() const noexcept override {
        return "This vehicle number is already registered.";
    }
};

class VehicleNotFoundException : public exception {
public:
    const char* what() const noexcept override {
        return "No vehicle found with that registration number.";
    }
};

string toUpper(string s) {
    for (size_t i = 0; i < s.length(); i++)
        s[i] = (char)toupper((unsigned char)s[i]);
    return s;
}

// Validates a registration number and returns it in upper case
string normalizeReg(string reg) {
    reg = toUpper(reg);
    if (reg.length() < 6 || reg.length() > 10) throw InvalidRegNumberException();
    for (size_t i = 0; i < reg.length(); i++)
        if (!isalnum((unsigned char)reg[i])) throw InvalidRegNumberException();
    return reg;
}

#endif
