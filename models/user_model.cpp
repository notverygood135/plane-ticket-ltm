#include <string>
#include <iostream>
#include <sqlite3.h>
#include <vector>
#include "../include/utils.hpp"
#include "../include/auth.hpp"
using namespace std;

int user_row_count = 0;
string user_rows;

static int callback(void *data, int argc, char **argv, char **column) {
    string row;
    if (user_row_count > 0) {
        row = ", {";
    }
    else {
        row = "{";
    }
    for (int i = 0; i < argc; i++) {
        row.append("\"");
        row.append(column[i]);
        row.append("\"");
        row.append(": \"");
        row.append(argv[i]);
        row.append("\"");
        if (i < argc - 1) {
            row.append(", ");
        }
    }
    row.append("}");
    cout << row << endl; 
    user_rows.append(row);
    user_row_count++;
    return 0;
}

vector<string> get_user(string username) {
    user_row_count = 0;
    user_rows = "[";
    sqlite3 *db;
    char *err_msg = 0;
    int rc;
    string sql;
    vector<string> response;

    rc = sqlite3_open("db/plane.db", &db);
    if (rc) {
        cout << "Cannot open database" << endl;
        response.push_back("HTTP/1.1 500 Internal Server Error\r\n\r\n");
        response.push_back("");
        return response;
    }
    sql = "SELECT username, money, type FROM users WHERE username = '";
    sql.append(username);
    sql.append("';");
    rc = sqlite3_exec(db, sql.c_str(), callback, NULL, &err_msg);
    if (rc != SQLITE_OK) {
        cout << "error: " << err_msg << endl;
        sqlite3_close(db);
        response.push_back("HTTP/1.1 500 Internal Server Error\r\n\r\n");
        response.push_back("");
        return response;
    }
    user_rows.append("]");
    sqlite3_close(db);
    response.push_back("HTTP/1.1 200 OK\r\n\r\n");
    response.push_back(user_rows);
    return response;
}

vector<string> get_passengers(string flight_id) {
    user_row_count = 0;
    user_rows = "[";
    sqlite3 *db;
    char *err_msg = 0;
    int rc;
    string sql;
    vector<string> response;

    rc = sqlite3_open("db/plane.db", &db);
    if (rc) {
        cout << "Cannot open database" << endl;
        response.push_back("HTTP/1.1 500 Internal Server Error\r\n\r\n");
        response.push_back("");
        return response;
    }
    sql = "SELECT username FROM own JOIN tickets ON own.ticket_id = tickets.ticket_id JOIN flights ON tickets.flight_id = flights.flight_id WHERE flights.flight_id = '";
    sql.append(flight_id);
    sql.append("';");
    rc = sqlite3_exec(db, sql.c_str(), callback, NULL, &err_msg);
    if (rc != SQLITE_OK) {
        cout << "error: " << err_msg << endl;
        sqlite3_close(db);
        response.push_back("HTTP/1.1 500 Internal Server Error\r\n\r\n");
        response.push_back("");
        return response;
    }
    user_rows.append("]");
    sqlite3_close(db);
    response.push_back("HTTP/1.1 200 OK\r\n\r\n");
    response.push_back(user_rows);
    return response;
}