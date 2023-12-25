#include <string>
#include <iostream>
#include <vector>
#include <sqlite3.h>
#include "../include/utils.hpp"
#include "../include/auth.hpp"
#include "../include/model.hpp"
using namespace std;

int notification_row_count = 0;
string notification_rows;

static int notificationsCallback(void *data, int argc, char **argv, char **column) {
    string row;
    if (notification_row_count > 0) {
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
    notification_rows.append(row);
    notification_row_count++;
    return 0;
}

static int notificationCallback(void *data, int argc, char **argv, char **column) {
    string row = "{";
    for (int i = 0; i < argc; i++) {
        printf("%s = %s\n", column[i], argv[i] ? argv[i] : "null");
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
    printf("\n");
    notification_rows.append(row);
    notification_row_count++;
    return 0;
}

vector<string> get_notifications(string username) {
        notification_row_count = 0;
    notification_rows = "[";
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
    sql = "SELECT username, \"content\", \"date\", \"time\" FROM notifications WHERE username = \"";
    sql.append(username);
    sql.append("\";");
    rc = sqlite3_exec(db, sql.c_str(), notificationsCallback, NULL, &err_msg);
    if (rc != SQLITE_OK) {
        cout << "error: " << err_msg << endl;
        response.push_back("HTTP/1.1 500 Internal Server Error\r\n\r\n");
        response.push_back("");
        return response;
    }
    notification_rows.append("]");
    sqlite3_close(db);
    response.push_back("HTTP/1.1 200 OK\r\n\r\n");
    response.push_back(notification_rows);
    return response;
}

vector<string> create_notification(string _username, string _content, string _date, string _time) {
    string username = split(_username, "=")[1];
    string content = split(_content, "=")[1];
    string date = split(_date, "=")[1];
    string time = split(_time, "=")[1];
    
    sqlite3 *db;
    char *err_msg = 0;
    notification_rows = "[";
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
    sql = "INSERT INTO notifications (username, content, date, time) VALUES ('";
    sql.append(username);
    sql.append("', '");
    sql.append(content);
    sql.append("', '");
    sql.append(date);
    sql.append("', '");
    sql.append(time);
    sql.append("');");
    rc = sqlite3_exec(db, sql.c_str(), notificationCallback, NULL, &err_msg);
    if (rc != SQLITE_OK) {
        cout << "error: " << err_msg << endl;
        response.push_back("HTTP/1.1 500 Internal Server Error\r\n\r\n");
        response.push_back("");
        return response;
    }
    notification_rows.append("]");
    sqlite3_close(db);
    response.push_back("HTTP/1.1 200 OK\r\n\r\n");
    response.push_back(notification_rows);
    return response;
}