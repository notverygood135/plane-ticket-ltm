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
    {"/register", "register"}
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
    }
    else if (route == "/login") {
        status = login(params[0], params[1]);
    }
    string html_path = "templates/";
    html_path.append(routes.at(route));
    html_path.append(".html");
    response.push_back(html_path);
    response.push_back(status);
    return response;
}

vector<string> get_route(char* method, char* route, char* payload) {
    printf("\nmethod: %s\nroute: %s\npayload: %s\n", method, route, payload);
    string m(method), r(route), p(payload);
    if (routes.find(r) != routes.end()) {
        if (m == "GET") return get(r);
        else if (m == "POST") return post(r, p);
    }
    cout << "unknown route: " << r << endl;
    vector<string> response = {"templates/error.html", "HTTP/1.1 404 Not Found\r\n\r\n"};
    return response;
}
