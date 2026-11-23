#include <iostream>
#include <algorithm>
#include <cctype>
#include "ExecutionEngine.h"
using namespace std;
namespace {
    string trim(string s) {
        size_t first = s.find_first_not_of(" \t\r\n");
        size_t last = s.find_last_not_of(" \t\r\n;");

        if (first == string::npos)
            return "";

        return s.substr(first, last - first + 1);
    }
    string upper(string s) {
        transform(s.begin(), s.end(), s.begin(),
            [](unsigned char c) { return toupper(c); });
        return s;
    }
    string removeQuotes(string s) {
        s = trim(s);
        if (s.size() >= 2 &&
            ((s.front() == '\'' && s.back() == '\'') ||
             (s.front() == '"' && s.back() == '"'))) {
            return s.substr(1, s.size() - 2);
        }
        return s;
    }
}
ExecutionEngine::ExecutionEngine() {
    currentTransactionID = -1;
    lastOperationSucceeded = false;
}
void ExecutionEngine::startTransaction() {
    int id = manager.createTransaction();

    activeTransactions.push_back(id);
    currentTransactionID = id;

    transactionUndoLogs[id].clear();
    savepoints.erase(id);

    scheduler.addTask(id);

    cout << "Current transaction: T" << id << endl;
}

void ExecutionEngine::switchTransaction(int transactionID) {
    for (int id : activeTransactions) {
        if (id == transactionID) {
            currentTransactionID = transactionID;

            cout << "Switched to transaction T"
                 << transactionID << endl;
            return;
        }
    }

    cout << "Transaction not found or already completed."
         << endl;
}

void ExecutionEngine::selectTransaction(int transactionID) {
    switchTransaction(transactionID);
}

void ExecutionEngine::executeSQL(const string& sql) {
    ParsedCommand command = parser.parse(sql);

    lastOperationSucceeded = false;

    if (command.type == BEGIN_CMD) {
        startTransaction();
        return;
    }

    if (command.type == SELECT_CMD) {
        executeSelect(sql);

        if (currentTransactionID != -1 &&
            lastOperationSucceeded) {
            manager.addCommand(currentTransactionID, command);
            cout << "Command processed for T"
                 << currentTransactionID << endl;
        }

        return;
    }

    if (currentTransactionID == -1) {
        cout << "Start a transaction first." << endl;
        return;
    }

    if (command.type == COMMIT_CMD) {
        commitTransaction();
        return;
    }

    if (command.type == ROLLBACK_CMD) {
        rollbackTransaction();
        return;
    }

    if (command.type == SAVEPOINT_CMD) {
        string name = trim(sql.substr(9));

        if (!name.empty()) {
            createSavepoint(name);
        }
        else {
            cout << "Invalid savepoint name." << endl;
        }

        return;
    }

    if (command.type == ROLLBACK_TO_SAVEPOINT_CMD) {
        string name = trim(sql.substr(11));

        if (!name.empty()) {
            rollbackToSavepoint(name);
        }
        else {
            cout << "Invalid savepoint name." << endl;
        }

        return;
    }

    if (command.type == INSERT_CMD) {
        executeInsert(sql);
    }
    else if (command.type == UPDATE_CMD) {
        executeUpdate(sql);
    }
    else if (command.type == DELETE_CMD) {
        executeDelete(sql);
    }
    else {
        cout << "Unsupported SQL command." << endl;
        return;
    }

    if (lastOperationSucceeded) {
        manager.addCommand(currentTransactionID, command);

        cout << "Command processed for T"
             << currentTransactionID << endl;
    }
}

void ExecutionEngine::undoOperationsFrom(size_t position) {
    int id = currentTransactionID;

    auto& log = transactionUndoLogs[id];

    if (position > log.size())
        return;

    for (int i = static_cast<int>(log.size()) - 1;
         i >= static_cast<int>(position); --i) {

        db.restoreTable(
            log[i].tableName,
            log[i].oldTable
        );
    }

    log.resize(position);
}

void ExecutionEngine::commitTransaction() {
    if (currentTransactionID == -1) {
        cout << "No active transaction selected." << endl;
        return;
    }

    int id = currentTransactionID;

    manager.updateStatus(id, COMMITTED);

    lockManager.releaseAllLocks(id);
    scheduler.removeTask(id);

    transactionUndoLogs.erase(id);
    savepoints.erase(id);

    activeTransactions.erase(
        remove(activeTransactions.begin(),
               activeTransactions.end(), id),
        activeTransactions.end()
    );

    cout << "Transaction T" << id
         << " committed." << endl;

    currentTransactionID = -1;
}

void ExecutionEngine::rollbackTransaction() {
    if (currentTransactionID == -1) {
        cout << "No active transaction selected." << endl;
        return;
    }

    int id = currentTransactionID;

    undoOperationsFrom(0);

    manager.updateStatus(id, ABORTED);

    lockManager.releaseAllLocks(id);
    scheduler.removeTask(id);

    transactionUndoLogs.erase(id);
    savepoints.erase(id);

    activeTransactions.erase(
        remove(activeTransactions.begin(),
               activeTransactions.end(), id),
        activeTransactions.end()
    );

    cout << "Transaction T" << id
         << " rolled back." << endl;

    currentTransactionID = -1;
}

void ExecutionEngine::showTransactions() {
    manager.showTransactions();

    if (currentTransactionID != -1) {
        cout << "Currently selected: T"
             << currentTransactionID << endl;
    }
    else {
        cout << "No transaction currently selected." << endl;
    }

    scheduler.showTasks();
    lockManager.showLocks();
}

void ExecutionEngine::createSavepoint(const string& name) {
    if (currentTransactionID == -1) {
        cout << "Start a transaction first." << endl;
        return;
    }

    savepoints[currentTransactionID][name] =
        transactionUndoLogs[currentTransactionID].size();

    cout << "Savepoint " << name
         << " created successfully." << endl;
}

void ExecutionEngine::rollbackToSavepoint(const string& name) {
    if (currentTransactionID == -1) {
        cout << "Start a transaction first." << endl;
        return;
    }

    auto transaction = savepoints.find(currentTransactionID);

    if (transaction == savepoints.end() ||
        transaction->second.find(name) == transaction->second.end()) {
        cout << "Savepoint not found." << endl;
        return;
    }

    size_t position = transaction->second[name];

    undoOperationsFrom(position);

    // Remove savepoints created after the selected savepoint
    for (auto it = transaction->second.begin();
         it != transaction->second.end();) {

        if (it->second > position)
            it = transaction->second.erase(it);
        else
            ++it;
    }

    cout << "Rolled back to savepoint "
         << name << "." << endl;
}

void ExecutionEngine::executeInsert(const string& sql) {
    string upperSQL = upper(sql);

    size_t intoPos = upperSQL.find("INTO");
    size_t valuesPos = upperSQL.find("VALUES");

    if (intoPos == string::npos ||
        valuesPos == string::npos) {
        cout << "Invalid INSERT syntax." << endl;
        return;
    }

    size_t colStart = sql.find('(', intoPos);
    size_t colEnd = sql.find(')', colStart);
    size_t valStart = sql.find('(', valuesPos);
    size_t valEnd = sql.find(')', valStart);

    if (colStart == string::npos ||
        colEnd == string::npos ||
        valStart == string::npos ||
        valEnd == string::npos) {
        cout << "Invalid INSERT syntax." << endl;
        return;
    }

    string tableName = trim(
        sql.substr(intoPos + 4, colStart - intoPos - 4)
    );

    string columns = sql.substr(
        colStart + 1, colEnd - colStart - 1
    );

    string values = sql.substr(
        valStart + 1, valEnd - valStart - 1
    );

    Database::Row row;

    size_t c = 0;
    size_t v = 0;

    while (c < columns.size() && v < values.size()) {
        size_t commaC = columns.find(',', c);
        size_t commaV = values.find(',', v);

        string column = trim(columns.substr(
            c,
            commaC == string::npos ? string::npos : commaC - c
        ));

        string value = removeQuotes(values.substr(
            v,
            commaV == string::npos ? string::npos : commaV - v
        ));

        if (column.empty()) {
            cout << "Invalid INSERT column." << endl;
            return;
        }

        transform(column.begin(), column.end(), column.begin(),
            [](unsigned char ch) { return tolower(ch); });

        row[column] = value;

        if (commaC == string::npos || commaV == string::npos)
            break;

        c = commaC + 1;
        v = commaV + 1;
    }

    if (!lockManager.acquireLock(
            currentTransactionID, tableName, Exclusive)) {
        return;
    }

    UndoOperation op;
    op.tableName = tableName;
    op.oldTable = db.snapshotTable(tableName);

    transactionUndoLogs[currentTransactionID].push_back(op);

    db.insertRow(tableName, row);

    lastOperationSucceeded = true;
}

void ExecutionEngine::executeSelect(const string& sql) {
    string upperSQL = upper(sql);

    size_t fromPos = upperSQL.find("FROM");

    if (fromPos == string::npos) {
        cout << "Invalid SELECT syntax." << endl;
        return;
    }

    string tableName = trim(sql.substr(fromPos + 4));

    if (tableName.empty()) {
        cout << "Invalid table name." << endl;
        return;
    }

    if (currentTransactionID != -1) {
        if (!lockManager.acquireLock(
                currentTransactionID, tableName, Shared)) {
            return;
        }
    }

    db.showTable(tableName);
    lastOperationSucceeded = true;
}


void ExecutionEngine::executeUpdate(const string& sql) {
    string upperSQL = upper(sql);

    size_t setPos = upperSQL.find("SET");
    size_t wherePos = upperSQL.find("WHERE");

    if (setPos == string::npos ||
        wherePos == string::npos) {
        cout << "Invalid UPDATE syntax." << endl;
        return;
    }

    string tableName = trim(
        sql.substr(6, setPos - 6)
    );

    string assignment = trim(
        sql.substr(setPos + 3, wherePos - setPos - 3)
    );

    string condition = trim(sql.substr(wherePos + 5));

    size_t equal1 = assignment.find('=');
    size_t equal2 = condition.find('=');

    if (equal1 == string::npos ||
        equal2 == string::npos) {
        cout << "Invalid UPDATE syntax." << endl;
        return;
    }

    string column = trim(assignment.substr(0, equal1));
    string value = removeQuotes(assignment.substr(equal1 + 1));

    string conditionColumn = trim(condition.substr(0, equal2));
    string conditionValue = removeQuotes(condition.substr(equal2 + 1));

    if (!lockManager.acquireLock(
            currentTransactionID, tableName, Exclusive)) {
        return;
    }

    UndoOperation op;
    op.tableName = tableName;
    op.oldTable = db.snapshotTable(tableName);

    lastOperationSucceeded = db.updateRow(
        tableName,
        column,
        value,
        conditionColumn,
        conditionValue
    );

    if (lastOperationSucceeded) {
        transactionUndoLogs[currentTransactionID].push_back(op);
    }
}
   
void ExecutionEngine::executeDelete(const string& sql) {
    string upperSQL = upper(sql);

    size_t fromPos = upperSQL.find("FROM");
    size_t wherePos = upperSQL.find("WHERE");

    if (fromPos == string::npos ||
        wherePos == string::npos) {
        cout << "Invalid DELETE syntax." << endl;
        return;
    }

    string tableName = trim(
        sql.substr(fromPos + 4, wherePos - fromPos - 4)
    );

    string condition = trim(sql.substr(wherePos + 5));

    size_t equalPos = condition.find('=');

    if (equalPos == string::npos) {
        cout << "Invalid DELETE condition." << endl;
        return;
    }

    string conditionColumn = trim(condition.substr(0, equalPos));
    string conditionValue = removeQuotes(condition.substr(equalPos + 1));

    if (!lockManager.acquireLock(
            currentTransactionID, tableName, Exclusive)) {
        return;
    }

    UndoOperation op;
    op.tableName = tableName;
    op.oldTable = db.snapshotTable(tableName);

    lastOperationSucceeded = db.deleteRow(
        tableName,
        conditionColumn,
        conditionValue
    );

    if (lastOperationSucceeded) {
        transactionUndoLogs[currentTransactionID].push_back(op);
    }
}
