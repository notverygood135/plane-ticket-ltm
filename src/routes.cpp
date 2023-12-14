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
    return response;
}

vector<string> post(string route, string payload) {
    vector<string> response;
    string status = "HTTP/1.1 500 Internal Server Error\r\n\r\n";
    vector<string> params = split(payload, "&");

    if (route == "/register") {
        status = _register(params[0], params[1], params[2]);
        response.push_back(status);
        if (status == "HTTP/1.1 200 OK\r\n\r\n") {
            response.push_back("OK");
        }
        else {
            response.push_back("NOT OK");
        }
    }
    else if (route == "/login") {
        status = login(params[0], params[1]);
        response.push_back(status);
        if (status == "HTTP/1.1 200 OK\r\n\r\n") {
            response.push_back(split(params[0], "=")[1]);
        }
        else {
            response.push_back("NOT OK");
        }
    }
    return response;
}

vector<string> req(string method, string route, string payload) {
    cout << "method: " << method << ", route: " << route << ", payload: " << payload << endl;
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

    cout << "unknown route: " << route << endl;
    response = {"templates/error.html", "HTTP/1.1 404 Not Found\r\n\r\n"};
    return response;
}
