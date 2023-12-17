#include <string>
#include <iostream>
#include <sqlite3.h>
#include "../include/utils.hpp"
#include "../include/model.hpp"
using namespace std;

int own_rows = 0;

static int callback(void *data, int argc, char **argv, char **column) {
    own_rows = argc;
    cout << "records found: " << argc << endl;
    return 0;
}

vector<string> create_ownership(string _username, string _ticket_id, string _full_name, string _number, string _security_code, string _expiration_date) {
    string username = split(_username, "=")[1];
    string ticket_id = split(_ticket_id, "=")[1];
    string full_name = split(_full_name, "=")[1];
    string number = split(_number, "=")[1];
    string security_code = split(_security_code, "=")[1];
    string expiration_date = split(_expiration_date, "=")[1];
    
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
    sql = "INSERT INTO own VALUES ('";
    sql.append(username);
    sql.append("', '");
    sql.append(ticket_id);
    sql.append("', '");
    sql.append(full_name);
    sql.append("', '");
    sql.append(number);
    sql.append("', '");
    sql.append(security_code);
    sql.append("', '");
    sql.append(expiration_date);
    sql.append("');");
    cout << sql << endl;
    rc = sqlite3_exec(db, sql.c_str(), NULL, 0, &err_msg);

    if (rc != SQLITE_OK) {
        cout << "error: " << err_msg << endl;
        response.push_back("HTTP/1.1 500 Internal Server Error\r\n\r\n");
        response.push_back("");
        return response;
    }
    cout << "successfully bought ticket " << username << endl;
    sqlite3_close(db);
    response.push_back("HTTP/1.1 200 OK\r\n\r\n");
    response.push_back("OK");
    return response;
}