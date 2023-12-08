#include <string>
#include <iostream>
#include <sqlite3.h>
#include "../include/utils.hpp"
#include "../include/auth.hpp"
#include "../include/model.hpp"
using namespace std;

int flight_rows;

static int callback(void *data, int argc, char **argv, char **column) {
    flight_rows = argc;
    for (int i = 0; i < argc; i++) {
        printf("%s = %s\n", column[i], argv[i] ? argv[i] : "null");
    }
    printf("\n");
    return 0;
}

string get_flights() {
    flight_rows = 0;
    sqlite3 *db;
    char *err_msg = 0;
    int rc;
    string sql;

    rc = sqlite3_open("db/plane.db", &db);
    if (rc) {
        cout << "Cannot open database" << endl;
        return "HTTP/1.1 500 Internal Server Error\r\n\r\n";
    }
    sql = "SELECT * FROM flights;";
    rc = sqlite3_exec(db, sql.c_str(), callback, NULL, &err_msg);
    if (rc != SQLITE_OK) {
        cout << "error: " << err_msg << endl;
        return "HTTP/1.1 500 Internal Server Error\r\n\r\n";
    }
    cout << "successfully fetched data" << endl;
    sqlite3_close(db);
    return "HTTP/1.1 200 OK\r\n\r\n";
}