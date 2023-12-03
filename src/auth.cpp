#include <string>
#include <iostream>
#include <sqlite3.h>
#include "../include/utils.hpp"
#include "../include/auth.hpp"
using namespace std;

string _register(string _username, string _password, string _confirm) {
    string username = split(_username, "=")[1];
    string password = split(_password, "=")[1];
    string confirm = split(_confirm, "=")[1];
    
    sqlite3 *db;
    char *err_msg = 0;
    int rc;
    string sql;

    rc = sqlite3_open("db/plane.db", &db);
    if (rc) {
        cout << "Cannot open database" << endl;
        return "HTTP/1.1 500 Internal Server Error\r\n\r\n";
    }
    sql = "INSERT INTO users VALUES ('";
    sql.append(username);
    sql.append("', '");
    sql.append(password);
    sql.append("');");
    cout << sql << endl;
    rc = sqlite3_exec(db, sql.c_str(), NULL, 0, &err_msg);

    if (rc != SQLITE_OK) {
        cout << "error: " << err_msg << endl;
        return "HTTP/1.1 500 Internal Server Error\r\n\r\n";
    }
    cout << "successfully registered user " << username << endl;
    sqlite3_close(db);
    return "HTTP/1.1 200 OK\r\n\r\n";
}

string login(string _username, string _password) {
    string username = split(_username, "=")[1];
    string password = split(_password, "=")[1];
    sqlite3 *db;
    char *err_msg = 0;
    int rc;
    string sql;

    rc = sqlite3_open("db/plane.db", &db);
    if (rc) {
        cout << "Cannot open database" << endl;
        return "HTTP/1.1 500 Internal Server Error\r\n\r\n";
    }
    return "HTTP/1.1 200 OK\r\n\r\n";
}