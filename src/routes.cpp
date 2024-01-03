#include "../include/routes.hpp"
#include "../include/utils.hpp"
#include "../include/auth.hpp"
#include "../include/model.hpp"
#include <unordered_map>
#include <iostream>
#include <string>
using namespace std;

unordered_map<string, string> template_routes = { 
    {"/", "index"}, 
    {"/login", "login"}, 
    {"/register", "register"},
    {"/flight", "flight"},
    {"/inventory", "inventory"},
    {"/notifications", "notifications"},
    {"/manage/flights", "index_admin"},
    {"/manage/users", "users_admin"},
    
   
};

vector<string> get(string route) {
    vector<string> response;
    vector<string> route_parse = split(route, "/");
    int params_count = route_parse.size();
    string parsed_route = route_parse[1];

    if (template_routes.find(route) != template_routes.end()) {
        string html_path = "templates/";
        html_path.append(template_routes.at(route));
        html_path.append(".html");
        response.push_back("HTTP/1.1 200 OK\r\n\r\n");
        response.push_back(html_path);
        return response;
    }
    
    if (parsed_route == "flights") {
        if (params_count == 2) {
            response = get_flights();
        }
        else {
            string flight_id = route_parse[route_parse.size() - 1];
            response = get_flight(flight_id);
        }
    }
    else if (parsed_route == "tickets") {
        string flight_id = route_parse[route_parse.size() - 1];
        response = get_tickets(flight_id);
    }
    else if (parsed_route == "flight") {
        response = {"HTTP/1.1 200 OK\r\n\r\n", "templates/flight.html"};
    }
    else if (parsed_route == "buy") {
        response = {"HTTP/1.1 200 OK\r\n\r\n", "templates/buy.html"};
    }
    else if (parsed_route == "info") {
        response = {"HTTP/1.1 200 OK\r\n\r\n", "templates/info.html"};
    }
    else if (parsed_route == "manage") {
        response = {"HTTP/1.1 200 OK\r\n\r\n", "templates/flight_admin.html"};
    }
    else if (parsed_route =="exchange") {
        response = {"HTTP/1.1 200 OK\r\n\r\n", "templates/exchange.html"};
    }
    else if (parsed_route == "ticket") {
        string ticket_id = route_parse[route_parse.size() - 1];
        response = get_ticket(ticket_id);
    }
    else if (parsed_route == "inventory") {
        string username = route_parse[route_parse.size() - 1];
        response = get_owned_tickets(username);
    }
    else if (parsed_route == "notification") {
        string username = route_parse[route_parse.size() - 1];
        response = get_notifications(username);
    }
    else if (parsed_route == "unread") {
        string username = route_parse[route_parse.size() - 1];
        response = get_unread_notifications_count(username);
    }
    else if (parsed_route == "user") {
        string username = route_parse[route_parse.size() - 1];
        response = get_user(username);
    }
    else if (parsed_route == "passengers") {
        string flight_id = route_parse[route_parse.size() - 1];
        response = get_passengers(flight_id);
    }
    else if (parsed_route == "users") {
        response = get_users();
    }
    else if (parsed_route == "own") {
        string ticket_id = route_parse[route_parse.size() - 1];
        response = get_ownership(ticket_id);
    }
    else {
        cout << "unknown route: " << route << endl;
        response = {"HTTP/1.1 404 Not Found\r\n\r\n", "templates/error.html"};
    }
    return response;
}

vector<string> post(string route, string payload) {
    vector<string> body = split(payload, "&");
    if (route == "/register") {
        return create_user(body[0], body[1], body[2]);
    }
    else if (route == "/login") {
        return login(body[0], body[1]);
    }
    else if (route == "/own") {
        return create_ownership(body[0], body[1], body[2], body[3], body[4], body[5]);
    }
    else if (route == "/notification") {
        return create_notification(body[0], body[1], body[2], body[3]);
    }
    return {"HTTP/1.1 500 Internal Server Error\r\n\r\n"};
}

vector<string> put(string route, string payload) {
    vector<string> route_parse = split(route, "/");
    string parsed_route = route_parse[1];
    vector<string> body = split(payload, "&");

    if (parsed_route == "read") {
        return update_notifications(route_parse[route_parse.size() - 1]);
    }
    else if (parsed_route == "users") {
        return update_type(body[0], body[1]);
    }
    else if (parsed_route == "flights") {
        return update_flight(body[0], body[1], body[2]);
    }
    return {"HTTP/1.1 500 Internal Server Error\r\n\r\n"};
}

vector<string> _delete(string route, string payload) {
    vector<string> response;
    vector<string> route_parse = split(route, "/");
    int params_count = route_parse.size();
    string parsed_route = route_parse[1];

    vector<string> body = split(payload, "&");

    if (parsed_route == "own") {
        update_money(body[1]);
        return delete_ownership(body[0]);
    }
    else if (parsed_route == "users") {
        string username = route_parse[route_parse.size() - 1];
        return delete_user(username);
    }
    else if (parsed_route == "notification") {
        string username = route_parse[route_parse.size() - 1];
        return delete_notifications(username);
    }
    return {"HTTP/1.1 500 Internal Server Error\r\n\r\n"};
}

vector<string> req(string method, string route, string payload) {
    // cout << "method: " << method << ", route: " << route << ", payload: " << payload << endl;
    vector<string> response;
    // neu client yeu cau file css
    if (route.find("css") != string::npos) {
        route = route.substr(1, route.length() - 1);
        response.push_back("HTTP/1.1 200 OK\r\n\r\n");
        response.push_back(route);
        return response;
    }
    if (method == "GET") return get(route);
    else if (method == "POST") return post(route, payload);
    else if (method == "PUT") return put(route, payload);
    else if (method == "DELETE") return _delete(route, payload);
    cout << "unknown method: " << route << endl;
    response = {"templates/error.html", "HTTP/1.1 404 Not Found\r\n\r\n"};
    return response;
}
