#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>

#include "../include/response.hpp"
#include "../include/routes.hpp"
#include <string>
#include <iostream>
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

    int client_socket;
    while (true) {
        // receive request from client
        char request[4096];
        client_socket = accept(server_socket, NULL, NULL);
        recv(client_socket, request, sizeof(request), 0);
        printf("request:\n%s\n", request);

        // process the headers of the request
        char *method = "";
		char *urlRoute = "";
        char *client_http_header = "";
        char *payload = "";

		char *request_parse = strtok(request, "\n");
        int request_parse_counter = 0;
		while (request_parse != NULL) {
            switch (request_parse_counter)
            {
            case 0:
                client_http_header = request_parse;
                break;
            default:
                payload = request_parse;
                break;
            }
            request_parse = strtok(NULL, "\n");
            request_parse_counter++;
        }
		
		char *header_token = strtok(client_http_header, " ");
		printf("\nheader_token: %s\n", header_token);

		int header_parse_counter = 0;
		while (header_token != NULL) {
			switch (header_parse_counter) {
				case 0:
					method = header_token;
				case 1:
					urlRoute = header_token;
			}
			header_token = strtok(NULL, " ");
			header_parse_counter++;
		}

        // respond to client
        vector<string> response = get_route(method, urlRoute, payload);
        string html_path = response[0];
        string http_header = response[1];
        
        // char* http_header = render_static_file(html_path);
        // printf("response: \n%s\n", http_header);

        // char http_header[4096] = "HTTP/1.1 200 OK\r\n\r\n";
        string response_data = render_static_file(html_path);
        http_header.append(response_data);
        http_header.erase(http_header.length() - 1);
        http_header.append("\r\n\r\n");
        // strcat(http_header, response_data);
        // strcat(http_header, "\r\n\r\n");
        cout << http_header << endl;

        send(client_socket, http_header.c_str(), sizeof(char) * http_header.length(), 0);

        // close connection
        close(client_socket);
        // free(response_data);
    }

    return 0;
}
