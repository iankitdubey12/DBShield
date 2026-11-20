#ifndef TRANSACTIONMANAGER_H
#define TRANSACTIONMANAGER_H
#include <vector>
#include "SQLParser.h"
using namespace std;
enum TransactionStatus {
    ACTIVE,
    COMMITTED,
    ABORTED
};
struct ManagedTransaction {
    int id;
    vector<ParsedCommand> commands;
    TransactionStatus status;
};
class TransactionManager {
    vector<ManagedTransaction> transactions;
    int nextID;
public:
    TransactionManager();
    int createTransaction();
    void addCommand(int transactionID, ParsedCommand command);
    void showTransactions();
    void updateStatus(int transactionID, TransactionStatus status);
};
#endif