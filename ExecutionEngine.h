#ifndef EXECUTIONENGINE_H
#define EXECUTIONENGINE_H

#include <string>
#include <vector>
#include <map>
#include <algorithm>

#include "SQLParser.h"
#include "TransactionManager.h"
#include "Database.h"
#include "LockManager.h"
#include "Scheduler.h"

using namespace std;

class ExecutionEngine {
    SQLParser parser;
    Database db;
    TransactionManager manager;

    LockManager lockManager;
    Scheduler scheduler;

    vector<int> activeTransactions;
    int currentTransactionID;

    struct UndoOperation {
        string tableName;
        Database::Table oldTable;
    };

    map<int, vector<UndoOperation>> transactionUndoLogs;
    map<int, map<string, size_t>> savepoints;

    bool lastOperationSucceeded;

    void undoOperationsFrom(size_t position);

public:
    ExecutionEngine();

    void startTransaction();
    void selectTransaction(int transactionID);
    void switchTransaction(int transactionID);

    void executeSQL(const string& sql);

    void commitTransaction();
    void rollbackTransaction();

    void createSavepoint(const string& name);
    void rollbackToSavepoint(const string& name);

    void showTransactions();

    void executeInsert(const string& sql);
    void executeSelect(const string& sql);
    void executeUpdate(const string& sql);
    void executeDelete(const string& sql);
};

#endif