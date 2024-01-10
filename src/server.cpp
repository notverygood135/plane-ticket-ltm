#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
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
    int i;
    // create a socket
    int server_socket;
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in client_address, server_address;
    bzero(&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8001);
    server_address.sin_addr.s_addr = INADDR_ANY;

    bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address));

    listen(server_socket, 5);

    cout << "server listening on port 8001" << endl;

    int client[FD_SETSIZE];
    int maxfd = server_socket;
    int maxi = -1;
    fd_set rset, allset;
    for (i = 0; i < FD_SETSIZE; i++)
		client[i] = -1;			/* -1 indicates available entry */
	FD_ZERO(&allset);
	FD_SET(server_socket, &allset);

    int client_socket, sockfd;
    int nready;
    socklen_t		clilen;
    while (true) {
        rset = allset;		/* structure assignment */
		nready = select(maxfd + 1, &rset, NULL, NULL, NULL);

        if (FD_ISSET(server_socket, &rset)) {	/* new client connection */
			clilen = sizeof(client_address);
			client_socket = accept(server_socket, (sockaddr*) &client_address, &clilen);

            for (i = 0; i < FD_SETSIZE; i++)
                if (client[i] < 0) {
                    client[i] = client_socket;	/* save descriptor */
                    break;
                }
            if (i == FD_SETSIZE) {
                cout << "Too many clients" << endl;
                return 0;
            }

            FD_SET(client_socket, &allset);	/* add new descriptor to set */
            if (client_socket > maxfd)
                maxfd = client_socket;			/* for select */
            if (i > maxi)
                maxi = i;				/* max index in client[] array */

            if (--nready <= 0)
                continue;				/* no more readable descriptors */
        }

        for (i = 0; i <= maxi; i++) {	/* check all clients for data */
			if ( (sockfd = client[i]) < 0)
				continue;
			if (FD_ISSET(sockfd, &rset)) {
                // receive request from client
                char _request[4096];
                memset(_request, 0, sizeof(_request));
				recv(client_socket, _request, sizeof(_request), 0);

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

				if (--nready <= 0)
					break;				/* no more readable descriptors */
			}
		}
    }
    return 0;
}
