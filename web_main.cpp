
#include <iostream>
#include <fstream>
#include <sstream>
#include <mutex>
#include <string>
#include <algorithm>
#include <cctype>

#include "httplib.h"
#include "ExecutionEngine.h"

using namespace std;

// Read frontend files
string readFile(const string& filename) {
    ifstream file(filename);

    if (!file.is_open()) {
        return "";
    }

    stringstream buffer;
    buffer << file.rdbuf();

    return buffer.str();
}

// Remove leading and trailing spaces
string trimCommand(string s) {
    size_t first = s.find_first_not_of(" \t\r\n");
    size_t last = s.find_last_not_of(" \t\r\n");

    if (first == string::npos) {
        return "";
    }

    return s.substr(first, last - first + 1);
}

// Convert string to uppercase
string toUpperCase(string s) {
    transform(
        s.begin(),
        s.end(),
        s.begin(),
        [](unsigned char c) {
            return static_cast<char>(toupper(c));
        }
    );

    return s;
}

int main() {

    httplib::Server server;

    ExecutionEngine engine;
    mutex engineMutex;

    // Serve HTML
    server.Get("/", [](const httplib::Request& req,
                       httplib::Response& res) {

        string page = readFile("index.html");

        if (page.empty()) {
            res.status = 404;
            res.set_content(
                "index.html not found",
                "text/plain"
            );
            return;
        }

        res.set_content(page, "text/html");
    });

    // Serve CSS
    server.Get("/style.css", [](const httplib::Request& req,
                                httplib::Response& res) {

        res.set_content(
            readFile("style.css"),
            "text/css"
        );
    });

    // Serve JavaScript
    server.Get("/script.js", [](const httplib::Request& req,
                                httplib::Response& res) {

        res.set_content(
            readFile("script.js"),
            "application/javascript"
        );
    });

    // SQL and Transaction Command Handler
    server.Post("/api/sql",
        [&](const httplib::Request& req,
            httplib::Response& res) {

            lock_guard<mutex> guard(engineMutex);

            ostringstream capturedOutput;

            streambuf* oldBuffer =
                cout.rdbuf(capturedOutput.rdbuf());

            try {

                string command = trimCommand(req.body);

                string upperCommand =
                    toUpperCase(command);

                cout << "Received command: "
                     << command << endl;

                // SWITCH T1 / SWITCH T2
                if (upperCommand.rfind("SWITCH ", 0) == 0) {

                    string transaction =
                        trimCommand(upperCommand.substr(7));

                    if (transaction.size() > 1 &&
                        transaction[0] == 'T' &&
                        all_of(
                            transaction.begin() + 1,
                            transaction.end(),
                            [](unsigned char c) {
                                return isdigit(c);
                            }
                        )) {

                        int id = stoi(transaction.substr(1));

                        engine.switchTransaction(id);
                    }
                    else {
                        cout << "Invalid SWITCH command."
                             << endl;
                    }
                }

                // SHOW TRANSACTIONS
                else if (upperCommand == "SHOW TRANSACTIONS") {

                    engine.showTransactions();
                }

                // Normal SQL commands
                else {

                    engine.executeSQL(command);
                }

            }
            catch (const exception& e) {

                cout << "Error: "
                     << e.what() << endl;
            }
            catch (...) {

                cout << "Unexpected error occurred."
                     << endl;
            }

            // Restore original cout
            cout.rdbuf(oldBuffer);

            // Send output to browser
            res.set_content(
                capturedOutput.str(),
                "text/plain"
            );
        }
    );

    cout << "DBShield Web Server Started!" << endl;
    cout << "Open http://localhost:8080" << endl;

    // Start server
    server.listen("127.0.0.1", 8080);

    return 0;
}
