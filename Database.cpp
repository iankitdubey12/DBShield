#include "Database.h"
#include <algorithm>
#include <cctype>

namespace {
    string normalize(string s) {
        transform(s.begin(), s.end(), s.begin(),
            [](unsigned char c) { return tolower(c); });
        return s;
    }
}

Database::Database() {
    insertRow("accounts", {{"id", "1"}, {"balance", "5000"}});
    insertRow("accounts", {{"id", "2"}, {"balance", "3000"}});
    insertRow("accounts", {{"id", "3"}, {"balance", "7000"}});
}

void Database::insertRow(const string& tableName, const Row& row) {
    Row normalizedRow;

    for (const auto& item : row)
        normalizedRow[normalize(item.first)] = item.second;

    tables[normalize(tableName)].push_back(normalizedRow);

    cout << "Row inserted successfully into "
         << normalize(tableName) << endl;
}

void Database::showTable(const string& tableName) {
    string name = normalize(tableName);

    if (tables.find(name) == tables.end()) {
        cout << "Table not found." << endl;
        return;
    }

    for (const Row& row : tables[name]) {
        for (const auto& column : row) {
            cout << column.first << ": "
                 << column.second << " | ";
        }
        cout << endl;
    }
}

bool Database::updateRow(
    const string& tableName,
    const string& column,
    const string& value,
    const string& conditionColumn,
    const string& conditionValue
) {
    string table = normalize(tableName);
    string col = normalize(column);
    string condition = normalize(conditionColumn);

    if (tables.find(table) == tables.end()) {
        cout << "Table not found." << endl;
        return false;
    }

    bool updated = false;

    for (Row& row : tables[table]) {
        auto it = row.find(condition);

        if (it != row.end() && it->second == conditionValue) {
            row[col] = value;
            updated = true;
        }
    }

    if (updated){
        cout << "Record updated successfully." << endl;
        return true;}

    else{
        cout << "No matching record found." << endl;
        return false;}
}

bool Database::deleteRow(
    const string& tableName,
    const string& conditionColumn,
    const string& conditionValue
) {
    string table = normalize(tableName);
    string condition = normalize(conditionColumn);

    if (tables.find(table) == tables.end()) {
        cout << "Table not found." << endl;
        return false;
    }

    auto& rows = tables[table];
    size_t oldSize = rows.size();

    rows.erase(
        remove_if(rows.begin(), rows.end(),
            [&](const Row& row) {
                auto it = row.find(condition);
                return it != row.end() &&
                       it->second == conditionValue;
            }),
        rows.end()
    );

    if (rows.size() < oldSize){
        cout << "Record deleted successfully." << endl;
        return true;}
    else{
        cout << "No matching record found." << endl;
        return false;}
}

Database::Table Database::snapshotTable(const string& tableName) {
    string table = normalize(tableName);

    if (tables.find(table) == tables.end())
        return {};

    return tables[table];
}

void Database::restoreTable(
    const string& tableName,
    const Table& snapshot
) {
    tables[normalize(tableName)] = snapshot;
    cout << "Table " << normalize(tableName)
         << " restored successfully." << endl;
}

map<string, Database::Table> Database::createSnapshot() {
    return tables;
}

void Database::restoreSnapshot(
    const map<string, Table>& snapshot
) {
    tables = snapshot;
    cout << "Database restored successfully." << endl;
}