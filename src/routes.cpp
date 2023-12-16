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
    else if (route.find("buy") != string::npos) {
        response = {"HTTP/1.1 200 OK\r\n\r\n", "templates/buy.html"};
    }
    else if (route.find("ticket") != string::npos) {
        vector<string> route_parse = split(route, "/");
        string id = route_parse[route_parse.size() - 1];
        cout << id << endl;
        response = get_ticket(id);
    }
    else {
        cout << "unknown route: " << route << endl;
        response = {"HTTP/1.1 404 Not Found\r\n\r\n", "templates/error.html"};
    }
    return response;
}

vector<string> post(string route, string payload) {
    vector<string> response;
    vector<string> params = split(payload, "&");

    if (route == "/register") {
        return create_user(params[0], params[1], params[2]);
    }
    if (route == "/login") {
        response = login(params[0], params[1]);
    }
    else if (route == "/buy") {
        response = create_ownership(params[0], params[1]);
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
    cout << "unknown method: " << route << endl;
    response = {"templates/error.html", "HTTP/1.1 404 Not Found\r\n\r\n"};
    return response;
}
