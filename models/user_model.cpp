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
    user_rows.append(row);
    user_row_count++;
    return 0;
}

vector<string> get_user(string email) {
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
    sql = "SELECT email, money, type, bonus FROM users WHERE email = '";
    sql.append(email);
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

vector<string> get_users() {
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
    sql = "SELECT email, type, bonus FROM users;";
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

vector<string> get_top_users() {
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
    sql = "SELECT users.email, COUNT(users.email) as count FROM own JOIN users ON own.email = users.email GROUP BY users.email ORDER BY count DESC;";
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
    sql = "SELECT email FROM own JOIN tickets ON own.ticket_id = tickets.ticket_id JOIN flights ON tickets.flight_id = flights.flight_id WHERE flights.flight_id = '";
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

vector<string> delete_user(string email) {
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
    sql = "DELETE FROM users WHERE email = '";
    sql.append(email);
    sql.append("';\n");
    sql.append("DELETE FROM notifications WHERE email = '");
    sql.append(email);
    sql.append("';\n");
    sql.append("UPDATE tickets SET owned = 0 WHERE ticket_id IN (SELECT ticket_id FROM own WHERE email = '");
    sql.append(email);
    sql.append("');\n");
    sql.append("DELETE FROM own WHERE email = '");
    sql.append(email);
    sql.append("';");
    cout << sql << endl;
    rc = sqlite3_exec(db, sql.c_str(), callback, NULL, &err_msg);
    if (rc != SQLITE_OK) {
        cout << "error: " << err_msg << endl;
        response.push_back("HTTP/1.1 500 Internal Server Error\r\n\r\n");
        response.push_back("");
        return response;
    }
    sqlite3_close(db);
    response.push_back("HTTP/1.1 200 OK\r\n\r\n");
    response.push_back("");
    return response;
}

vector<string> update_type(string _email, string _type) {
    string email = split(_email, "=")[1];
    string type = split(_type, "=")[1];
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
    sql = "UPDATE users SET type = ";
    sql.append(type);
    sql.append(" WHERE email = '");
    sql.append(email);
    sql.append("';");
    cout << sql << endl;
    rc = sqlite3_exec(db, sql.c_str(), callback, NULL, &err_msg);
    if (rc != SQLITE_OK) {
        cout << "error: " << err_msg << endl;
        response.push_back("HTTP/1.1 500 Internal Server Error\r\n\r\n");
        response.push_back("");
        return response;
    }
    sqlite3_close(db);
    response.push_back("HTTP/1.1 200 OK\r\n\r\n");
    response.push_back("");
    return response;
}

vector<string> update_money(string _email, string _money) {
    string email = split(_email, "=")[1];
    string money = split(_money, "=")[1];
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
    sql = "UPDATE users SET money = money - ";
    sql.append(money);
    sql.append(" WHERE email = '");
    sql.append(email);
    sql.append("';");
    cout << sql << endl;
    rc = sqlite3_exec(db, sql.c_str(), callback, NULL, &err_msg);
    if (rc != SQLITE_OK) {
        cout << "error: " << err_msg << endl;
        response.push_back("HTTP/1.1 500 Internal Server Error\r\n\r\n");
        response.push_back("");
        return response;
    }
    sqlite3_close(db);
    response.push_back("HTTP/1.1 200 OK\r\n\r\n");
    response.push_back("");
    return response;
}

vector<string> update_bonus(string _email, string _bonus) {
    string email = split(_email, "=")[1];
    string bonus = split(_bonus, "=")[1];
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
    sql = "UPDATE users SET bonus = ";
    sql.append(bonus);
    sql.append(" WHERE email = '");
    sql.append(email);
    sql.append("';");
    cout << sql << endl;
    rc = sqlite3_exec(db, sql.c_str(), callback, NULL, &err_msg);
    if (rc != SQLITE_OK) {
        cout << "error: " << err_msg << endl;
        response.push_back("HTTP/1.1 500 Internal Server Error\r\n\r\n");
        response.push_back("");
        return response;
    }
    sqlite3_close(db);
    response.push_back("HTTP/1.1 200 OK\r\n\r\n");
    response.push_back("");
    return response;
}