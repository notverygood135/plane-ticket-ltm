#include <string>
#include <iostream>
#include <vector>
#include <sqlite3.h>
#include "../include/utils.hpp"
#include "../include/auth.hpp"
#include "../include/model.hpp"
using namespace std;

int flight_row_count = 0;
string flight_rows;

static int flightsCallback(void *data, int argc, char **argv, char **column) {
    string row;
    if (flight_row_count > 0) {
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
    flight_rows.append(row);
    flight_row_count++;
    return 0;
}

vector<string> get_flights() {
    flight_row_count = 0;
    flight_rows = "[";
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
    sql = "SELECT * from flights";
    rc = sqlite3_exec(db, sql.c_str(), flightsCallback, NULL, &err_msg);
    if (rc != SQLITE_OK) {
        cout << "error: " << err_msg << endl;
        response.push_back("HTTP/1.1 500 Internal Server Error\r\n\r\n");
        response.push_back("");
        return response;
    }
    flight_rows.append("]");
    sqlite3_close(db);
    response.push_back("HTTP/1.1 200 OK\r\n\r\n");
    response.push_back(flight_rows);
    return response;
}

vector<string> get_flight(string flight_id) {
    flight_row_count = 0;
    flight_rows = "[";
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
    sql = "SELECT * from flights WHERE flight_id = \"";
    sql.append(flight_id);
    sql.append("\";");
    rc = sqlite3_exec(db, sql.c_str(), flightsCallback, NULL, &err_msg);
    if (rc != SQLITE_OK) {
        cout << "error: " << err_msg << endl;
        response.push_back("HTTP/1.1 500 Internal Server Error\r\n\r\n");
        response.push_back("");
        return response;
    }
    flight_rows.append("]");
    sqlite3_close(db);
    response.push_back("HTTP/1.1 200 OK\r\n\r\n");
    response.push_back(flight_rows);
    return response;
}

vector<string> update_flight(string _flight_id, string _date, string _time) {
    string flight_id = split(_flight_id, "=")[1];
    string date = split(_date, "=")[1];
    string time = split(_time, "=")[1];

    flight_row_count = 0;
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
    
    sql = "UPDATE flights SET 'date' = '";
    sql.append(date);
    sql.append("', 'time' = '");
    sql.append(time);
    sql.append("' WHERE flight_id = '");
    sql.append(flight_id);
    sql.append("';");
    cout << sql << endl;
    rc = sqlite3_exec(db, sql.c_str(), flightsCallback, NULL, &err_msg);
    if (rc != SQLITE_OK) {
        cout << "error: " << err_msg << endl;
        sqlite3_close(db);
        response.push_back("HTTP/1.1 500 Internal Server Error\r\n\r\n");
        response.push_back("");
        return response;
    }
    sqlite3_close(db);
    response.push_back("HTTP/1.1 200 OK\r\n\r\n");
    response.push_back("");
    return response;
}

vector<string> delete_flight(string _flight_id) {
    string flight_id = split(_flight_id, "=")[1];
    flight_row_count = 0;
    flight_rows = "[";
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
    sql = "DELETE from own WHERE flight_id = '";
    sql.append(flight_id);
    sql.append("';\n");
    sql = "DELETE from tickets WHERE flight_id = '";
    sql.append(flight_id);
    sql.append("';\n");
    sql.append("DELETE from flights WHERE flight_id = '");
    sql.append(flight_id);
    sql.append("';");
    cout << sql << endl;
    rc = sqlite3_exec(db, sql.c_str(), flightsCallback, NULL, &err_msg);
    if (rc != SQLITE_OK) {
        cout << "error: " << err_msg << endl;
        response.push_back("HTTP/1.1 500 Internal Server Error\r\n\r\n");
        response.push_back("");
        return response;
    }
    flight_rows.append("]");
    sqlite3_close(db);
    response.push_back("HTTP/1.1 200 OK\r\n\r\n");
    response.push_back(flight_rows);
    return response;
}