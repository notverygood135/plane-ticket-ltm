#include <string>
#include <iostream>
#include <vector>
#include <sqlite3.h>
#include "../include/utils.hpp"
#include "../include/auth.hpp"
#include "../include/model.hpp"
using namespace std;

int ticket_row_count = 0;
string ticket_rows;

static int ticketsCallback(void *data, int argc, char **argv, char **column) {
    string row;
    if (ticket_row_count > 0) {
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
    ticket_rows.append(row);
    ticket_row_count++;
    return 0;
}

static int ticketCallback(void *data, int argc, char **argv, char **column) {
    string row = "{";
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
    ticket_rows.append(row);
    ticket_row_count++;
    return 0;
}

vector<string> get_tickets(string flight_id) {
    ticket_row_count = 0;
    ticket_rows = "[";
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
    // sql = "SELECT flights.flight_id, \"from\", \"to\", \"date\", \"time\", airline, tickets.ticket_id, seat, price from flights JOIN tickets ON flights.flight_id = tickets.flight_id WHERE tickets.ticket_id NOT IN (SELECT ticket_id FROM own) AND tickets.flight_id = '";
    sql = "SELECT * from tickets WHERE flight_id = '";
    sql.append(flight_id);
    sql.append("';");
    rc = sqlite3_exec(db, sql.c_str(), ticketsCallback, NULL, &err_msg);
    if (rc != SQLITE_OK) {
        cout << "error: " << err_msg << endl;
        sqlite3_close(db);
        response.push_back("HTTP/1.1 500 Internal Server Error\r\n\r\n");
        response.push_back("");
        return response;
    }
    ticket_rows.append("]");
    sqlite3_close(db);
    response.push_back("HTTP/1.1 200 OK\r\n\r\n");
    response.push_back(ticket_rows);
    return response;
}

vector<string> get_owned_tickets(string username) {
    ticket_row_count = 0;
    ticket_rows = "[";
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
    sql = "SELECT flights.flight_id, \"from\", \"to\", \"date\", \"time\", airline, tickets.ticket_id, seat, price FROM flights JOIN tickets ON flights.flight_id = tickets.flight_id JOIN own ON own.ticket_id = tickets.ticket_id WHERE own.username = '";
    sql.append(username);
    sql.append("';");
    rc = sqlite3_exec(db, sql.c_str(), ticketsCallback, NULL, &err_msg);
    if (rc != SQLITE_OK) {
        cout << "error: " << err_msg << endl;
        sqlite3_close(db);
        response.push_back("HTTP/1.1 500 Internal Server Error\r\n\r\n");
        response.push_back("");
        return response;
    }
    ticket_rows.append("]");
    sqlite3_close(db);
    response.push_back("HTTP/1.1 200 OK\r\n\r\n");
    response.push_back(ticket_rows);
    return response;
}

vector<string> get_ticket(string id) {
    sqlite3 *db;
    char *err_msg = 0;
    ticket_rows = "[";
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
    sql = "SELECT * FROM flights JOIN tickets ON flights.flight_id = tickets.flight_id WHERE ticket_id = '";
    sql.append(id);
    sql.append("';");
    rc = sqlite3_exec(db, sql.c_str(), ticketCallback, NULL, &err_msg);
    if (rc != SQLITE_OK) {
        cout << "error: " << err_msg << endl;
        sqlite3_close(db);
        response.push_back("HTTP/1.1 500 Internal Server Error\r\n\r\n");
        response.push_back("");
        return response;
    }
    ticket_rows.append("]");
    sqlite3_close(db);
    response.push_back("HTTP/1.1 200 OK\r\n\r\n");
    response.push_back(ticket_rows);
    return response;
}