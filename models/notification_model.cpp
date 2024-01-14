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

vector<string> get_notifications(string email) {
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
    sql = "SELECT email, \"content\", \"date\", \"time\" FROM notifications WHERE email = \"";
    sql.append(email);
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

vector<string> get_unread_notifications_count(string email) {
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
    sql = "SELECT COUNT(notification_id) as count FROM notifications WHERE email = '";
    sql.append(email);
    sql.append("' AND read = 0;");
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

vector<string> update_notifications(string email) {
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
    sql = "UPDATE notifications SET read = 1 WHERE email = '";
    sql.append(email);
    sql.append("';");
    rc = sqlite3_exec(db, sql.c_str(), notificationsCallback, NULL, &err_msg);
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

vector<string> create_notification(string _email, string _content, string _date, string _time) {
    string email = split(_email, "=")[1];
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
    sql = "INSERT INTO notifications (email, content, date, time, read) VALUES ('";
    sql.append(email);
    sql.append("', '");
    sql.append(content);
    sql.append("', '");
    sql.append(date);
    sql.append("', '");
    sql.append(time);
    sql.append("', 0);");
    cout << sql << endl;
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

vector<string> delete_notifications(string email) {
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
    sql = "DELETE FROM notifications WHERE email = '";
    sql.append(email);
    sql.append("';");
    rc = sqlite3_exec(db, sql.c_str(), notificationsCallback, NULL, &err_msg);
    if (rc != SQLITE_OK) {
        cout << "error: " << err_msg << endl;
        response.push_back("HTTP/1.1 500 Internal Server Error\r\n\r\n");
        response.push_back("");
        return response;
    }
    sqlite3_close(db);
    response.push_back("HTTP/1.1 200 OK\r\n\r\n");
    response.push_back("OK");
    return response;
}