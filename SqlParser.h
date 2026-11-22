#ifndef SQLPARSER_H
#define SQLPARSER_H
#include <string>
using namespace std;
enum SQLCommandType {
    BEGIN_CMD,
    SELECT_CMD,
    INSERT_CMD,
    UPDATE_CMD,
    DELETE_CMD,
    COMMIT_CMD,
    ROLLBACK_CMD,
    ROLLBACK_TO_SAVEPOINT_CMD,
    SAVEPOINT_CMD,
    UNKNOWN_CMD
};
struct ParsedCommand {
    SQLCommandType type;
    string sql;
};
class SQLParser {
public:
    ParsedCommand parse(const string& sql);
};
#endif