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
    {"/inventory", "inventory"}
};

vector<string> get(string route) {
    vector<string> response;
    if (template_routes.find(route) != template_routes.end()) {
        string html_path = "templates/";
        html_path.append(template_routes.at(route));
        html_path.append(".html");
        response.push_back("HTTP/1.1 200 OK\r\n\r\n");
        response.push_back(html_path);
        return response;
    }
    if (route == "/tickets") {
        response = get_tickets();
    }
    else if (route.find("buy") != string::npos) {
        response = {"HTTP/1.1 200 OK\r\n\r\n", "templates/buy.html"};
    }
    else if (route.find("info") != string::npos) {
        response = {"HTTP/1.1 200 OK\r\n\r\n", "templates/info.html"};
    }
    else if (route.find("ticket") != string::npos) {
        vector<string> route_parse = split(route, "/");
        string id = route_parse[route_parse.size() - 1];
        response = get_ticket(id);
    }
    else if (route.find("inventory") != string::npos) {
        vector<string> route_parse = split(route, "/");
        string username = route_parse[route_parse.size() - 1];
        response = get_owned_tickets(username);
    }
    else {
        cout << "unknown route: " << route << endl;
        response = {"HTTP/1.1 404 Not Found\r\n\r\n", "templates/error.html"};
    }
    return response;
}

vector<string> post(string route, string payload) {
    vector<string> params = split(payload, "&");
    if (route == "/register") {
        return create_user(params[0], params[1], params[2]);
    }
    if (route == "/login") {
        return login(params[0], params[1]);
    }
    else if (route == "/own") {
        return create_ownership(params[0], params[1], params[2], params[3], params[4], params[5]);
    }
    return {"HTTP/1.1 500 Internal Server Error\r\n\r\n"};
}

vector<string> _delete(string route, string payload) {
    vector<string> params = split(payload, "&");
    if (route.find("/own") != string::npos) {
        return delete_ownership(params[0]);
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
    else if (method == "DELETE") return _delete(route, payload);
    cout << "unknown method: " << route << endl;
    response = {"templates/error.html", "HTTP/1.1 404 Not Found\r\n\r\n"};
    return response;
}
