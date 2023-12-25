#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>

#include <string>
#include <iostream>
#include <chrono>

#include "../include/response.hpp"
#include "../include/routes.hpp"
#include "../include/utils.hpp"
using namespace std;

int main() {
    // create a socket
    int server_socket;
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_address;
    bzero(&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8001);
    server_address.sin_addr.s_addr = INADDR_ANY;

    bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address));

    listen(server_socket, 5);

    cout << "server listening on port 8001" << endl;

    int client_socket;
    while (true) {
        // receive request from client
        char _request[4096];
        memset(_request, 0, sizeof(_request));
        client_socket = accept(server_socket, NULL, NULL);
        recv(client_socket, _request, sizeof(_request), 0);
        // printf("request:\n%s\n", _request);
        if (!strcmp(_request, "\0")) {
            continue;
        };

        // process the headers of the request
        string request(_request);
        log(request);
        vector<string> request_parse = split(request, "\n");
        string client_http_header = request_parse.front();
        string payload = request_parse.back();

        vector<string> client_http_header_parse = split(client_http_header, " ");
        string method = client_http_header_parse[0];
		string urlRoute = client_http_header_parse[1];

        // respond to client
        vector<string> response = req(method, urlRoute, payload);
        string http_header = response[0]; // HTTP status
        string html_path = response[1]; // html path or response from request
        if (html_path.find("html") != string::npos || html_path.find("css") != string::npos) { // if the response is of type html
            string response_data = render_static_file(html_path); // render the html page
            http_header.append(response_data);
            http_header.erase(http_header.length() - 1);
        }
        else {
            http_header.append(html_path); // response from the server
        }
        http_header.append("\r\n\r\n");
        log(http_header);
        send(client_socket, http_header.c_str(), sizeof(char) * http_header.length(), 0);

        // close connection
        close(client_socket);
    }

    return 0;
}
