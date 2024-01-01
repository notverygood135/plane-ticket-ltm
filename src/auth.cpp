#include <string>
#include <iostream>
#include <sqlite3.h>
#include <vector>
#include "../include/utils.hpp"
#include "../include/auth.hpp"
using namespace std;

string rows = "";
int row_count = 0;

static int callback(void *data, int argc, char **argv, char **column) {
    string row;
    if (row_count > 0) {
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
    rows.append(row);
    row_count++;
    return 0;
}

vector<string> create_user(string _username, string _password, string _confirm) {
    string username = split(_username, "=")[1];
    string password = split(_password, "=")[1];
    string confirm = split(_confirm, "=")[1];
    
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
    sql = "INSERT INTO users VALUES ('";
    sql.append(username);
    sql.append("', '");
    sql.append(password);
    sql.append("', 10000.0, 0);");
    cout << sql << endl;
    rc = sqlite3_exec(db, sql.c_str(), NULL, 0, &err_msg);

    if (rc != SQLITE_OK) {
        cout << "error: " << err_msg << endl;
        response.push_back("HTTP/1.1 500 Internal Server Error\r\n\r\n");
        response.push_back("");
        return response;
    }
    cout << "successfully registered user " << username << endl;
    sqlite3_close(db);
    response.push_back("HTTP/1.1 200 OK\r\n\r\n");
    response.push_back("OK");
    return response;
}

vector<string> login(string _username, string _password) {
    row_count = 0;
    rows = "[";
    string username = split(_username, "=")[1];
    string password = split(_password, "=")[1];

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
    sql = "SELECT * FROM users WHERE username = '";
    sql.append(username);
    sql.append("' AND password = '");
    sql.append(password);
    sql.append("';");
    rc = sqlite3_exec(db, sql.c_str(), callback, NULL, &err_msg);
    if (rc != SQLITE_OK) {
        cout << "error: " << err_msg << endl;
        response.push_back("HTTP/1.1 500 Internal Server Error\r\n\r\n");
        response.push_back("");
        return response;
    }
    if (!row_count) {
        cout << "username or password is incorrect" << endl;
        sqlite3_close(db);
        response.push_back("HTTP/1.1 401 Unauthorized\r\n\r\n");
        response.push_back("");
        return response;
    }
    cout << "successfully logged in user " << username << endl;
    sqlite3_close(db);
    rows.append("]");
    response.push_back("HTTP/1.1 200 OK\r\n\r\n");
    response.push_back(rows);
    return response;
}