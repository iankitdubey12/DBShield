#include <iostream>
#include "TransactionManager.h"
using namespace std;
TransactionManager::TransactionManager() {
    nextID = 1;
}
int TransactionManager::createTransaction() {
    ManagedTransaction t;
    t.id = nextID++;
    t.status = ACTIVE;
    transactions.push_back(t);
    cout << "Transaction T" << t.id
         << " created successfully." << endl;
    return t.id;
}
void TransactionManager::addCommand(int transactionID, ParsedCommand command) {
    for (ManagedTransaction& t : transactions) {
        if (t.id == transactionID && t.status == ACTIVE) {
            t.commands.push_back(command);
            return;
        }
    }
    cout << "Active transaction not found." << endl;
}
void TransactionManager::showTransactions() {
    for (const ManagedTransaction& t : transactions) {
        cout << "Transaction T" << t.id << " | ";
        if (t.status == ACTIVE)
            cout << "ACTIVE";
        else if (t.status == COMMITTED)
            cout << "COMMITTED";
        else
            cout << "ABORTED";
        cout << " | Commands: " << t.commands.size() << endl;
    }
}
void TransactionManager::updateStatus(int transactionID, TransactionStatus status) {
    for (ManagedTransaction& t : transactions) {
        if (t.id == transactionID) {
            t.status = status;
            return;
        }
    }
}