// =====================================================================
// main.cpp  —  MEMBER 4: AKHIL (final integration of all 4 modules)
//
// Build:  g++ -std=c++11 -Wall -Wextra main.cpp -o traffic
// Run  :  ./traffic
//
// Input note: names and phone numbers must be ONE word (use _ for spaces).
// =====================================================================
#include "Payment.h"

const string DATA_FILE = "records.txt";

void showMenu() {
    cout << endl;
    cout << "===== TRAFFIC FINE & VIOLATION SYSTEM =====" << endl;
    cout << "1. Register vehicle" << endl;
    cout << "2. Log violation" << endl;
    cout << "3. Search vehicle (details + history)" << endl;
    cout << "4. Pay oldest pending fine" << endl;
    cout << "5. Summary report" << endl;
    cout << "0. Save & exit" << endl;
    cout << "Enter choice: ";
}

int main() {
    HashTable table;             // Abhay - vehicle storage
    PendingQueue pending;        // Akhil - pending fines

    if (loadFromFile(table, DATA_FILE))
        cout << "Loaded " << table.size() << " vehicle(s) from " << DATA_FILE << endl;
    else
        cout << "No saved data found. Starting fresh." << endl;

    int choice = -1;
    do {
        showMenu();
        if (!(cin >> choice)) {          // user typed a non-number
            cin.clear();
            cin.ignore(1000, '\n');
            choice = -1;
        }

        try {
            switch (choice) {
                case 1: {
                    int type;
                    string reg, ownerName, phone;
                    cout << "Type (1=Two-Wheeler, 2=Four-Wheeler, 3=Commercial): ";
                    cin >> type;
                    cout << "Registration number: ";
                    cin >> reg;
                    cout << "Owner name (one word): ";
                    cin >> ownerName;
                    cout << "Phone: ";
                    cin >> phone;
                    registerVehicle(table, type, reg, Owner(ownerName, phone));   // Abhay
                    break;
                }
                case 2: {
                    string reg, date;
                    int vtype;
                    cout << "Registration number: ";
                    cin >> reg;
                    cout << "Violation (1=Speeding, 2=Signal Jump, 3=No Helmet, 4=Wrong Parking): ";
                    cin >> vtype;
                    cout << "Date (YYYY-MM-DD): ";
                    cin >> date;
                    Violation* v = createViolation(vtype, date);                  // Rakshit
                    if (v == nullptr) {
                        cout << "Invalid violation type." << endl;
                        break;
                    }
                    logViolation(table, reg, v);                                  // Divyanshi
                    break;
                }
                case 3: {
                    string reg;
                    cout << "Registration number: ";
                    cin >> reg;
                    findVehicle(table, reg)->showDetails();                       // Divyanshi
                    break;
                }
                case 4:
                    buildPendingQueue(table, pending);                            // Akhil
                    cout << "Pending fines in queue: " << pending.size() << endl;
                    processNextPending(pending);
                    break;
                case 5:
                    generateReport(table);                                        // Akhil
                    break;
                case 0:
                    break;
                default:
                    cout << "Invalid choice. Try again." << endl;
            }
        } catch (exception& e) {
            cout << "Error: " << e.what() << endl;
        }
    } while (choice != 0);

    if (saveToFile(table, DATA_FILE))
        cout << "Records saved to " << DATA_FILE << ". Goodbye!" << endl;
    else
        cout << "Warning: could not save records." << endl;

    return 0;
}
