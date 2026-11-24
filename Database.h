
#ifndef DATABASE_H
#define DATABASE_H
#include <iostream>
#include <string>
#include <vector>
#include <map>
using namespace std;
class Database {
public:
    using Row = map<string, string>;
    using Table = vector<Row>;
private:
    map<string, vector<Row>> tables;
public:
    Database();
    void insertRow(const string& tableName, const Row& row);
    void showTable(const string& tableName);
    bool updateRow(
        const string& tableName,
        const string& column,
        const string& value,
        const string& conditionColumn,
        const string& conditionValue
    );
    bool deleteRow(
        const string& tableName,
        const string& conditionColumn,
        const string& conditionValue
    );
    bool findRow(
        const string& tableName,
        const string& conditionColumn,
        const string& conditionValue,
        Row& oldRow
    );
    void restoreRow(
        const string& tableName,
        const string& conditionColumn,
        const string& conditionValue,
        const Row& oldRow
    );
    void removeRow(
        const string& tableName,
        const string& conditionColumn,
        const string& conditionValue
    );
    Table snapshotTable(const string& tableName);

void restoreTable(
    const string& tableName,
    const Table& snapshot
);

map<string, Table> createSnapshot();

void restoreSnapshot(
    const map<string, Table>& snapshot);
};
#endif
