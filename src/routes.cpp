#include "../include/routes.hpp"
#include "../include/utils.hpp"
#include "../include/auth.hpp"
#include <unordered_map>
#include <iostream>
#include <string>
using namespace std;

unordered_map<string, string> routes = { 
    {"/", "index"}, 
    {"/login", "login"}, 
    {"/register", "register"},
    {"/home", "home"}
};

vector<string> get(string route) {
    vector<string> response;
    string html_path = "templates/";
    html_path.append(routes.at(route));
    html_path.append(".html");
    response.push_back(html_path);
    response.push_back("HTTP/1.1 200 OK\r\n\r\n");
    return response;
}

vector<string> post(string route, string payload) {
    vector<string> response;
    string status = "HTTP/1.1 500 Internal Server Error\r\n\r\n";
    vector<string> params = split(payload, "&");

    if (route == "/register") {
        status = _register(params[0], params[1], params[2]);
        if (status == "HTTP/1.1 200 OK\r\n\r\n") {
            response.push_back("OK");
        }
        else {
            response.push_back("NOT OK");
        }
        response.push_back(status);
    }
    else if (route == "/login") {
        status = login(params[0], params[1]);
        if (status == "HTTP/1.1 200 OK\r\n\r\n") {
            response.push_back(split(params[0], "=")[1]);
        }
        else {
            response.push_back("");
        }
        response.push_back(status);
    }
    return response;
}

vector<string> req(string method, string route, string payload) {
    cout << "method: " << method << ", route: " << route << ", payload: " << payload << endl;
    if (routes.find(route) != routes.end()) {
        if (method == "GET") return get(route);
        else if (method == "POST") return post(route, payload);
    }
    vector<string> response;
    if (route.find("css") != string::npos) {
        route = route.substr(1, route.length() - 1);
        response = {route, "HTTP/1.1 200 OK\r\n\r\n"};
        return response;
    }
    cout << "unknown route: " << route << endl;
    response = {"templates/error.html", "HTTP/1.1 404 Not Found\r\n\r\n"};
    return response;
}
