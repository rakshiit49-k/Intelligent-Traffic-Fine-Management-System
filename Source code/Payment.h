#ifndef PAYMENT_H
#define PAYMENT_H

#include <fstream>
#include "Records.h"


// DSA: QUEUE (FIFO) of pending fines
struct PendingItem {
    Vehicle* vehicle;
    Violation* violation;
};

struct QueueNode {
    PendingItem item;
    QueueNode* next;
};

class PendingQueue {
private:
    QueueNode* front;
    QueueNode* rear;
    int count;

public:
    PendingQueue() : front(nullptr), rear(nullptr), count(0) {}
    ~PendingQueue() { clear(); }

    PendingQueue(const PendingQueue&) = delete;
    PendingQueue& operator=(const PendingQueue&) = delete;

    bool isEmpty() { return front == nullptr; }
    int size() { return count; }

    void enqueue(Vehicle* v, Violation* viol) {
        QueueNode* node = new QueueNode();
        node->item.vehicle = v;
        node->item.violation = viol;
        node->next = nullptr;
        if (rear == nullptr) front = rear = node;
        else {
            rear->next = node;
            rear = node;
        }
        count++;
    }

    bool dequeue(Vehicle*& v, Violation*& viol) {
        if (front == nullptr) return false;
        QueueNode* temp = front;
        v = temp->item.vehicle;
        viol = temp->item.violation;
        front = front->next;
        if (front == nullptr) rear = nullptr;
        delete temp;
        count--;
        return true;
    }

    void clear() {
        Vehicle* v;
        Violation* viol;
        while (dequeue(v, viol)) {}
    }
};

// Collects every UNPAID fine, sorts them oldest-first by date
// (insertion sort), and loads them into the queue.
void buildPendingQueue(HashTable& table, PendingQueue& q) {
    q.clear();
    int n = table.size();
    if (n == 0) return;

    Vehicle** vehicles = new Vehicle*[n];
    int got = table.getAllVehicles(vehicles, n);

    int total = 0;
    for (int i = 0; i < got; i++)
        for (ViolationNode* cur = vehicles[i]->getViolationHead(); cur != nullptr; cur = cur->next)
            if (!cur->data->isPaid()) total++;

    if (total > 0) {
        PendingItem* items = new PendingItem[total];
        int k = 0;
        for (int i = 0; i < got; i++)
            for (ViolationNode* cur = vehicles[i]->getViolationHead(); cur != nullptr; cur = cur->next)
                if (!cur->data->isPaid()) {
                    items[k].vehicle = vehicles[i];
                    items[k].violation = cur->data;
                    k++;
                }

        // insertion sort by date (YYYY-MM-DD compares correctly as text)
        for (int i = 1; i < total; i++) {
            PendingItem key = items[i];
            int j = i - 1;
            while (j >= 0 && items[j].violation->getDate() > key.violation->getDate()) {
                items[j + 1] = items[j];
                j--;
            }
            items[j + 1] = key;
        }

        for (int i = 0; i < total; i++) q.enqueue(items[i].vehicle, items[i].violation);
        delete[] items;
    }
    delete[] vehicles;
}

// Pays the OLDEST pending fine
void processNextPending(PendingQueue& q) {
    Vehicle* v;
    Violation* viol;
    if (!q.dequeue(v, viol)) {
        cout << "No pending fines." << endl;
        return;
    }
    viol->markPaid();
    cout << "Paid: " << viol->getName() << " (" << viol->getDate() << ") for "
         << v->getRegNumber() << " - Rs." << viol->getFinalFine() << endl;
}


// FILE HANDLING
bool saveToFile(HashTable& table, const string& filename) {
    ofstream out(filename.c_str());
    if (!out) return false;

    int n = table.size();
    out << n << "\n";
    if (n > 0) {
        Vehicle** arr = new Vehicle*[n];
        int got = table.getAllVehicles(arr, n);
        for (int i = 0; i < got; i++) {
            Vehicle* v = arr[i];
            out << v->getTypeCode() << " " << v->getRegNumber() << " "
                << v->getOwnerName() << " " << v->getOwnerPhone() << " "
                << v->getViolationCount() << "\n";
            for (ViolationNode* cur = v->getViolationHead(); cur != nullptr; cur = cur->next)
                out << cur->data->getTypeCode() << " " << cur->data->getDate() << " "
                    << cur->data->getFinalFine() << " " << (cur->data->isPaid() ? 1 : 0) << "\n";
        }
        delete[] arr;
    }
    return true;
}

// Returns false if the file does not exist or is unreadable
bool loadFromFile(HashTable& table, const string& filename) {
    ifstream in(filename.c_str());
    if (!in) return false;

    int n;
    if (!(in >> n)) return false;

    for (int i = 0; i < n; i++) {
        int type, vcount;
        string reg, ownerName, phone;
        if (!(in >> type >> reg >> ownerName >> phone >> vcount)) return false;

        Vehicle* veh = createVehicle(type, reg, Owner(ownerName, phone));
        if (veh == nullptr) return false;

        for (int j = 0; j < vcount; j++) {
            int vtype, fine, paidFlag;
            string date;
            if (!(in >> vtype >> date >> fine >> paidFlag)) {
                delete veh;
                return false;
            }
            Violation* viol = createViolation(vtype, date);
            if (viol == nullptr) continue;
            viol->setFinalFine(fine);
            viol->setPaid(paidFlag == 1);
            veh->restoreViolation(viol);     
        }
        if (!table.insert(veh)) delete veh;
    }
    return true;
}


// SUMMARY REPORT

void generateReport(HashTable& table) {
    int n = table.size();
    int issuedCount = 0, issuedAmt = 0, collected = 0, pending = 0;

    if (n > 0) {
        Vehicle** arr = new Vehicle*[n];
        int got = table.getAllVehicles(arr, n);
        for (int i = 0; i < got; i++)
            for (ViolationNode* cur = arr[i]->getViolationHead(); cur != nullptr; cur = cur->next) {
                issuedCount++;
                issuedAmt += cur->data->getFinalFine();
                if (cur->data->isPaid()) collected += cur->data->getFinalFine();
                else pending += cur->data->getFinalFine();
            }
        delete[] arr;
    }

    cout << "========== SUMMARY REPORT ==========" << endl;
    cout << "Vehicles registered : " << n << endl;
    cout << "Violations issued   : " << issuedCount << endl;
    cout << "Total fines issued  : Rs." << issuedAmt << endl;
    cout << "Collected (paid)    : Rs." << collected << endl;
    cout << "Pending (unpaid)    : Rs." << pending << endl;
    cout << "====================================" << endl;
}

#endif
