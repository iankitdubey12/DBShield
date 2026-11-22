#include <iostream>
#include <algorithm>
#include <cctype>
#include "SQLParser.h"
using namespace std;
ParsedCommand SQLParser::parse(const string& sql) {
    string command = sql;
    command.erase(0, command.find_first_not_of(" \t\n\r"));
    transform(command.begin(), command.end(), command.begin(),
        [](unsigned char c) {
            return toupper(c);
        });
    ParsedCommand result;
    result.sql = sql;
    if (command.find("BEGIN") == 0)
        result.type = BEGIN_CMD;
    if (command.compare(0, 5, "BEGIN") == 0 &&
    (command.length() == 5 || command[5] == ' ' || command[5] == '\t'|| command[5] == ';'))
    result.type = BEGIN_CMD;

else if (command.compare(0, 6, "SELECT") == 0 &&
    (command.length() == 6 || command[6] == ' ' || command[6] == '\t'))
    result.type = SELECT_CMD;

else if (command.compare(0, 6, "INSERT") == 0 &&
    (command.length() == 6 || command[6] == ' ' || command[6] == '\t'))
    result.type = INSERT_CMD;

else if (command.compare(0, 6, "UPDATE") == 0 &&
    (command.length() == 6 || command[6] == ' ' || command[6] == '\t'))
    result.type = UPDATE_CMD;

else if (command.compare(0, 6, "DELETE") == 0 &&
    (command.length() == 6 || command[6] == ' ' || command[6] == '\t'))
    result.type = DELETE_CMD;

else if (command.compare(0, 6, "COMMIT") == 0 &&
    (command.length() == 6 || command[6] == ' ' || command[6] == '\t' || command[6] == ';'))
    result.type = COMMIT_CMD;

else if (command.compare(0, 11, "ROLLBACK TO") == 0 &&
    (command.length() == 11 || command[11] == ' ' || command[11] == '\t'))
    result.type = ROLLBACK_TO_SAVEPOINT_CMD;

else if (command.compare(0, 8, "ROLLBACK") == 0 &&
    (command.length() == 8 || command[8] == ' ' || command[8] == '\t' || command[8] == ';'))
    result.type = ROLLBACK_CMD;

else if (command.compare(0, 9, "SAVEPOINT") == 0 &&
    (command.length() == 9 || command[9] == ' ' || command[9] == '\t'))
    result.type = SAVEPOINT_CMD;
else
    result.type = UNKNOWN_CMD;
return result;
}