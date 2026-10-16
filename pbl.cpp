// TRAFFIC FINE & VIOLATION MANAGEMENT SYSTEM

#include <iostream>
#include <string>
#include <fstream>

using namespace std;

// OWNER MODULE

class Owner 
{
private:
    string name;
    string contact;
    string dlNumber;

public:
    Owner(string name, string contact, string dlNumber)
    {
        this->name= name;
        this->contact= contact;
        this->dlNumber= dlNumber;
    } 
    
    string getName() 
    {
         return name; 
    }

    string getContact() { return contact; }
    string getDlNumber() { return dlNumber; }

    void displayOwner() 
    {
        cout<<"Owner: "<<name<<" | Contact: "<<contact<<" | DLNumber: "<<dlNumber<<endl;
    }
};


// VIOLATION MODULE

class Violation 
{
protected:
    string date;
    bool isPaid;
    double baseFineAmount;

public:
    Violation(string date, double baseFineAmount) 
    {
        this->date = date;
        this->baseFineAmount = baseFineAmount;
        this->isPaid = false;
    }

    virtual ~Violation() {}
    
    virtual double calculateFine() = 0; 
    
    virtual string getViolationType() = 0; 

    void markAsPaid() 
    { 
        isPaid = true; 
    }

    bool getPaymentStatus() 
    { 
        return isPaid; 
    }

    string getDate() 
    { 
        return date; 
    }

    double getBaseFine() 
    { 
        return baseFineAmount; 
    }
};

class SpeedViolation : public Violation 
{
private:
    int overSpeedLimit;

public:
    SpeedViolation(string date, double baseFineAmount, int overSpeedLimit) : Violation(date, baseFineAmount) 
    {
        this->overSpeedLimit = overSpeedLimit;
    }
    
    double calculateFine() override 
    {
         return baseFineAmount + (overSpeedLimit * 10); 
    }
    string getViolationType() override 
    { 
        return "Speeding"; 
    }
};

class NoHelmet : public Violation 
{
public:
    NoHelmet(string date, double baseFineAmount) : Violation(date, baseFineAmount) {}

    double calculateFine() override 
    { 
        return baseFineAmount; 
    }

    string getViolationType() override 
    { 
        return "No Helmet"; 
    }
};

class NoSeatbeltViolation : public Violation 
{
public:
    NoSeatbeltViolation(string date, double baseFineAmount) : Violation(date, baseFineAmount) {}
    
    double calculateFine() override 
    { 
        return baseFineAmount; 
    }

    string getViolationType() override 
    { 
        return "No Seatbelt"; 
    }
};

class OverPassengerViolation : public Violation 
{
private:
    int extraPassengers;

public:
    OverPassengerViolation(string date, double baseFineAmount, int extraPassengers) : Violation(date, baseFineAmount)
    {
        this->extraPassengers = extraPassengers;
    } 
        
    double calculateFine() override 
    { 
        return baseFineAmount + (extraPassengers * 500); 
    }

    string getViolationType() override 
    { 
        return "Overpassenger"; 
    }
};

class TrafficLightViolation : public Violation 
{
public:
    TrafficLightViolation(string date, double baseFineAmount) : Violation(date, baseFineAmount) {}
    
    double calculateFine() override 
    { 
        return baseFineAmount; 
    }

    string getViolationType() override 
    { 
        return "Red Light Jump"; 
    }
};

class OverloadViolation : public Violation 
{
private:
    double extraTonnage;

public:
    OverloadViolation(string date, double baseFineAmount, double extraTonnage) : Violation(date, baseFineAmount)
    {
        this->extraTonnage = extraTonnage;
    } 
        
    double calculateFine() override 
    { 
        return baseFineAmount + (extraTonnage * 1000); 
    }

    string getViolationType() override 
    { 
        return "Overloaded Vehicle"; 
    }
};


// VIOLATION LINKED LIST MODULE

struct ViolationNode 
{
    Violation* data;
    ViolationNode* next;

    ViolationNode(Violation* data) 
    {
        this->data= data;
        next= NULL;
    }
};

class ViolationLinkedList 
{
private:
    ViolationNode* head;
    int totalViolations;
    int unpaidViolations;
    double totalUnpaidAmount;

public:
    ViolationLinkedList() 
    {
        head= NULL;
        totalViolations = 0;
        unpaidViolations = 0;
        totalUnpaidAmount = 0;
    } 

    ~ViolationLinkedList() 
    {
        ViolationNode* current = head;
        while (current != NULL) 
        {
            ViolationNode* next = current->next;
            delete current->data;
            delete current;
            current = next;
        }
    }

    void insertViolation(Violation* data) 
    {
        ViolationNode* newNode = new ViolationNode(data);
        
        newNode->next = head;
        head = newNode;

        totalViolations++;
        unpaidViolations++;
        totalUnpaidAmount += data->calculateFine();
    }

    double getTotalUnpaidFines() 
    {
        return totalUnpaidAmount;
    }

    int getViolationCount() 
    {
        return totalViolations;
    }

    int getUnpaidViolationCount() 
    {
        return unpaidViolations;
    }

    void processPayments() 
    {
        if (unpaidViolations == 0) 
        {
            cout << "No pending fines to process!" << endl;
            return;
        }

        ViolationNode* temp = head;

        while (temp != NULL) 
        {
            if (!temp->data->getPaymentStatus()) 
            {
                temp->data->markAsPaid();
                cout << "Processed payment of: Rs. " << temp->data->calculateFine() 
                     << " (Date: " << temp->data->getDate() << ")" << endl;
            }
            temp = temp->next;
        }

        unpaidViolations = 0;
        totalUnpaidAmount = 0;
        cout << "All dues cleared for this vehicle." << endl;
    }
    
    ViolationNode* getHead() 
    {
        return head; 
    }
};


// VEHICLE MODULE

class Vehicle 
{
protected:
    string regNumber;
    Owner* vehicleOwner;
    string vehicleType;
    ViolationLinkedList* history; 

public:
    Vehicle(string regNumber, Owner* vehicleOwner, string vehicleType) 
    {
        this->regNumber= regNumber;
        this->vehicleOwner= vehicleOwner;
        this->vehicleType= vehicleType;
        this->history= new ViolationLinkedList();
    }

    virtual ~Vehicle() 
    { 
        delete history; 
        delete vehicleOwner;
    }

    string getRegNumber() 
    { 
        return regNumber; 
    }

    string getVehicleType() 
    { 
        return vehicleType; 
    }

    Owner* getOwner() 
    { 
        return vehicleOwner; 
    }

    ViolationLinkedList* getHistory() 
    { 
        return history; 
    }
    
    void logViolation(Violation* data) 
    {
        history->insertViolation(data);
    }
    
    double getPendingFines() 
    {
        return history->getTotalUnpaidFines();
    }

    virtual void displayDetails() = 0; 
};

class TwoWheeler : public Vehicle 
{
public:
    TwoWheeler(string regNumber, Owner* vehicleOwner) : Vehicle(regNumber, vehicleOwner, "Two Wheeler") {}

    void displayDetails() override 
    {
        cout << "Vehicle Type: " << vehicleType << endl;
        cout << "Registration Number: " << regNumber << endl;
        vehicleOwner->displayOwner();
    }
};

class FourWheeler : public Vehicle 
{
public:
    FourWheeler(string regNumber, Owner* vehicleOwner) : Vehicle(regNumber, vehicleOwner, "Four Wheeler") {}

    void displayDetails() override 
    {
        cout << "Vehicle Type: " << vehicleType << endl;
        cout << "Registration Number: " << regNumber << endl;
        vehicleOwner->displayOwner();
    }
};

class CommercialVehicle : public Vehicle 
{
private:
    double loadCapacity; 

public:
    CommercialVehicle(string regNumber, Owner* vehicleOwner, double loadCapacity) : Vehicle(regNumber, vehicleOwner, "Commercial") 
    {
        this->loadCapacity= loadCapacity;
    }

    void displayDetails() override 
    {
        cout << "Vehicle Type: " << vehicleType << endl;
        cout << "Registration Number: " << regNumber << endl;
        cout << "Load Capacity: " << loadCapacity << " Tons\n";
        vehicleOwner->displayOwner();
    }
    
    double getLoadCapacity() 
    { 
        return loadCapacity; 
    }
};


// HASH TABLE MODULE

struct VehicleHashNode 
{
    Vehicle* vehicle;
    VehicleHashNode* next;

    VehicleHashNode(Vehicle* vehicle) 
    {
        this->vehicle= vehicle;
        next= NULL;
    }
};

class VehicleHashTable 
{
private:
    int tableSize;
    int currentElements;
    VehicleHashNode** table;

    int hashFunction(string regNumber) 
    {
        int hash = 0;
        for (char c : regNumber) 
        {
            hash = (hash + c) % tableSize;
        }
        return hash;
    }

    void rehash() 
    {
        int oldTableSize = tableSize;
        VehicleHashNode** oldTable = table;

        tableSize = tableSize * 2; 
        table = new VehicleHashNode*[tableSize];
        
        for (int i = 0; i < tableSize; i++) 
        {
            table[i] = NULL;
        }

        currentElements = 0; 

        for (int i = 0; i < oldTableSize; i++) 
        {
            VehicleHashNode* temp = oldTable[i];
            while (temp != NULL) 
            {
                VehicleHashNode* nextNode = temp->next;
                insertVehicle(temp->vehicle); 
                delete temp; 
                temp = nextNode;
            }
        }
        delete[] oldTable; 
    }

public:
    VehicleHashTable() 
    {
        tableSize = 16; 
        currentElements = 0;
        table = new VehicleHashNode*[tableSize];
        
        for (int i = 0; i < tableSize; i++) 
        {
            table[i] = NULL;
        }
    }

    ~VehicleHashTable() 
    {
        for (int i = 0; i < tableSize; i++) 
        {
            VehicleHashNode* temp = table[i];
            while (temp != NULL) 
            {
                VehicleHashNode* nextNode = temp->next;
                delete temp->vehicle; 
                delete temp;
                temp = nextNode;
            }
        }
        delete[] table;
    }

    void insertVehicle(Vehicle* vehicle) 
    {
        int index = hashFunction(vehicle->getRegNumber());
        VehicleHashNode* newNode = new VehicleHashNode(vehicle);
        
        if (table[index] == NULL) 
        {
            table[index] = newNode;
        } 
        else 
        {
            VehicleHashNode* temp = table[index];
            while (temp->next != NULL) 
            {
                temp = temp->next;
            }
            temp->next = newNode;
        }
        currentElements++;

        // Checking for rehashing
        double loadFactor = (double)currentElements / tableSize;
        if (loadFactor >= 0.75) 
        {
            rehash();
        }
    }

    Vehicle* searchVehicle(string regNumber) 
    {
        int index = hashFunction(regNumber);
        VehicleHashNode* temp = table[index];
        
        while (temp != NULL) 
        {
            if (temp->vehicle->getRegNumber() == regNumber) 
            {
                return temp->vehicle;
            }
            temp = temp->next;
        }
        return NULL; 
    }

    VehicleHashNode** getTable() 
    { 
        return table; 
    }
    int getTableSize() 
    { 
        return tableSize; 
    }
};

// TEMPORARY MAIN FUNCTION

int main() 
{
    VehicleHashTable db;
    int choice;

    do 
    {
        cout << "\n=======================================" << endl;
        cout << "  TRAFFIC VIOLATION SYSTEM" << endl;
        cout << "=======================================" << endl;
        cout << "1. Register a New Vehicle" << endl;
        cout << "2. Log a Traffic Violation" << endl;
        cout << "3. Search Vehicle & View Pending Fines" << endl;
        cout << "4. Process Payments" << endl;
        cout << "5. View System Analytics" << endl;
        cout << "6. Exit" << endl;
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) 
        {
            case 1: 
            {
                string name, contact, dl, regNum;
                int typeChoice;
                
                cout << "\n--- Register Vehicle ---" << endl;
                cout << "Owner Name: "; cin >> name;
                cout << "Owner Contact: "; cin >> contact;
                cout << "Owner DL Number: "; cin >> dl;
                
                Owner* newOwner = new Owner(name, contact, dl);
                
                cout << "Vehicle Reg Number (e.g., UK071234): "; cin >> regNum;
                cout << "Type (1: 2-Wheeler, 2: 4-Wheeler, 3: Commercial): "; cin >> typeChoice;
                
                Vehicle* newVehicle = NULL;
                if (typeChoice == 1) newVehicle = new TwoWheeler(regNum, newOwner);
                else if (typeChoice == 2) newVehicle = new FourWheeler(regNum, newOwner);
                else if (typeChoice == 3) 
                {
                    double capacity;
                    cout << "Enter Load Capacity (Tons): "; cin >> capacity;
                    newVehicle = new CommercialVehicle(regNum, newOwner, capacity);
                }
                
                if (newVehicle != NULL) 
                {
                    db.insertVehicle(newVehicle);
                    cout << "Vehicle successfully registered!" << endl;
                }
                break;
            }
            case 2: 
            {
                string regNum, date;
                int vTypeChoice;
                
                cout << "\n--- Log Violation ---" << endl;
                cout << "Enter Vehicle Reg Number: "; cin >> regNum;
                
                Vehicle* v = db.searchVehicle(regNum);
                if (v == NULL) 
                {
                    cout << "Vehicle not found!" << endl;
                    break;
                }

                // Check for unpaid voilation penalty
                int unpaidViolations = v->getHistory()->getUnpaidViolationCount();
                double repeatOffenderPenalty = 0;
                
                if (unpaidViolations >= 2) 
                {
                    cout << "\nALERT: Defaulter Detected (" << unpaidViolations << " unpaid violations) !!" << endl;
                    cout << "Applying Rs. 500 Surcharge due to uncleared dues." << endl;
                    repeatOffenderPenalty = 500;
                }

                cout << "Violation Date (DD/MM/YYYY): "; cin >> date;
                string type = v->getVehicleType();
                Violation* newViolation = NULL;
                
                // TWO WHEELER MENU
                if (type == "Two Wheeler") 
                {
                    cout << "\nViolations for Two Wheeler:" << endl;
                    cout << "1. Speeding\n2. No Helmet\n3. Overpassenger\n4. Traffic Light\nChoice: ";
                    cin >> vTypeChoice;
                    
                    if (vTypeChoice == 1) 
                    {
                        int overSpeed; cout << "Km/h over the limit: "; cin >> overSpeed;
                        newViolation = new SpeedViolation(date, 1000 + repeatOffenderPenalty, overSpeed); 
                    } 
                    else if (vTypeChoice == 2) 
                    {
                        newViolation = new NoHelmet(date, 500 + repeatOffenderPenalty); 
                    }
                    else if (vTypeChoice == 3) 
                    {
                        int extra; cout << "Extra Passengers: "; cin >> extra;
                        newViolation = new OverPassengerViolation(date, 500 + repeatOffenderPenalty, extra); 
                    }
                    else if (vTypeChoice == 4) 
                    {
                        newViolation = new TrafficLightViolation(date, 1000 + repeatOffenderPenalty);
                    }
                }
                // FOUR WHEELER MENU
                else if (type == "Four Wheeler") 
                {
                    cout << "\nViolations for Four Wheeler:" << endl;
                    cout << "1. Speeding\n2. No Seatbelt\n3. Overpassenger\n4. Traffic Light\nChoice: ";
                    cin >> vTypeChoice;
                    
                    if (vTypeChoice == 1) 
                    {
                        int overSpeed; cout << "Km/h over the limit: "; cin >> overSpeed;
                        newViolation = new SpeedViolation(date, 1000 + repeatOffenderPenalty, overSpeed); 
                    } 
                    else if (vTypeChoice == 2) 
                    {
                        newViolation = new NoSeatbeltViolation(date, 1000 + repeatOffenderPenalty); 
                    }
                    else if (vTypeChoice == 3) 
                    {
                        int extra; cout << "Extra Passengers: "; cin >> extra;
                        newViolation = new OverPassengerViolation(date, 500 + repeatOffenderPenalty, extra); 
                    }
                    else if (vTypeChoice == 4) 
                    {
                        newViolation = new TrafficLightViolation(date, 1000 + repeatOffenderPenalty);
                    }
                }
                // COMMERCIAL VEHICLE MENU 
                else if (type == "Commercial") 
                {
                    cout << "\nViolations for Commercial Vehicle:" << endl;
                    cout << "1. Speeding\n2. No Seatbelt\n3. Overpassenger\n4. Traffic Light\n5. Overload\nChoice: ";
                    cin >> vTypeChoice;
                    
                    if (vTypeChoice == 1) 
                    {
                        int overSpeed; cout << "Km/h over the limit: "; cin >> overSpeed;
                        newViolation = new SpeedViolation(date, 2000 + repeatOffenderPenalty, overSpeed); 
                    } 
                    else if (vTypeChoice == 2) 
                    {
                        newViolation = new NoSeatbeltViolation(date, 1000 + repeatOffenderPenalty); 
                    }
                    else if (vTypeChoice == 3) 
                    {
                        int extra; cout << "Extra Passengers: "; cin >> extra;
                        newViolation = new OverPassengerViolation(date, 500 + repeatOffenderPenalty, extra); 
                    }
                    else if (vTypeChoice == 4) 
                    {
                        newViolation = new TrafficLightViolation(date, 2000 + repeatOffenderPenalty);
                    }
                    else if (vTypeChoice == 5)
                    {
                        double extraTonnage; cout << "Extra Tonnage: "; cin >> extraTonnage;
                        newViolation = new OverloadViolation(date, 5000 + repeatOffenderPenalty, extraTonnage);
                    }
                }

                if (newViolation != NULL) 
                {
                    v->logViolation(newViolation);
                    cout << "Violation logged successfully! Fine imposed on vehicle." << endl;
                } 
                else 
                {
                    cout << "Invalid choice! Violation not logged." << endl;
                }
                break;
            }
            case 3: 
            {
                string regNum;
                cout << "\n--- Search Vehicle ---" << endl;
                cout << "Enter Vehicle Reg Number: "; cin >> regNum;
                
                Vehicle* v = db.searchVehicle(regNum);
                if (v != NULL) 
                {
                    cout << "\n-- Vehicle Record Found --" << endl;
                    v->displayDetails();
                    
                    cout << "\n--- Violation History ---" << endl;
                    ViolationNode* temp = v->getHistory()->getHead();
                    if (temp == NULL) 
                    {
                        cout << "No violations found. Clean record!" << endl;
                    } 
                    else 
                    {
                        int ticketNum = 1;
                        while (temp != NULL) 
                        {
                            cout << "Ticket #" << ticketNum 
                                 << " | Type: " << temp->data->getViolationType() 
                                 << " | Date: " << temp->data->getDate() 
                                 << " | Fine: Rs. " << temp->data->calculateFine() 
                                 << " | Status: " << (temp->data->getPaymentStatus() ? "PAID" : "UNPAID") << endl;
                            temp = temp->next;
                            ticketNum++;
                        }
                    }
                    cout << "-------------------------" << endl;
                    cout << "Total Pending Fines: Rs. " << v->getPendingFines() << endl;
                } 
                else 
                {
                    cout << "Vehicle not found in Hash Table!" << endl;
                }
                break;
            }
            case 4: 
            {
                string regNum;
                cout << "\n--- Process Payments ---" << endl;
                cout << "Enter Vehicle Reg Number: "; cin >> regNum;
                
                Vehicle* v = db.searchVehicle(regNum);
                if (v != NULL) 
                {
                    cout << "\n--- Processing Payments for " << v->getRegNumber() << " ---" << endl;
                    v->getHistory()->processPayments();
                } 
                else 
                {
                    cout << "Vehicle not found!" << endl;
                }
                break;
            }
            case 5:
            {
                double totalCollected = 0;
                double totalPending = 0;

                for (int i = 0; i < db.getTableSize(); i++) 
                {
                    VehicleHashNode* temp = db.getTable()[i];
                    while (temp != NULL) 
                    {
                        ViolationNode* vNode = temp->vehicle->getHistory()->getHead();
                        while (vNode != NULL) 
                        {
                            if (vNode->data->getPaymentStatus()) 
                            {
                                totalCollected += vNode->data->calculateFine();
                            }
                            else 
                            {
                                totalPending += vNode->data->calculateFine();
                            }
                            vNode = vNode->next;
                        }
                        temp = temp->next;
                    }
                }

                cout << "\n--- SYSTEM ANALYTICS ---" << endl;
                cout << "Total Revenue Collected: Rs. " << totalCollected << endl;
                cout << "Total Outstanding Fines: Rs. " << totalPending << endl;
                cout << "Total System Value: Rs. " << (totalCollected + totalPending) << endl;
                break;
            }
            case 6:
                cout << "Exiting System...\n" << endl;
                break;
            default:
                cout << "Invalid choice. Try again." << endl;
        }
    } while (choice != 6);

    return 0;
}