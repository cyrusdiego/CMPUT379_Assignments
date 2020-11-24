/*
	C socket server example
*/

#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
#include <string>
#include <vector>
using namespace std;
// #include "tands.h"

bool is_valid_port(string port) {
    int p = stoi(port);
    return p >= 5000 && p <= 64000;
}

/**
 * Ensures:
 * - Valid number of input provided
 * - validate if passed arguments are valid port
 */
string parse_input(int argc, char **argv) {
    if (argc > 2 || argc <= 1 || !is_valid_port(argv[1])) {
        perror("Invalid arguments");
        exit(-1);
    }
    return argv[1];
}

int main(int argc, char *argv[]) {
    string port = parse_input(argc, argv);

    // code from eClass, modified to accept port from input
    struct sockaddr_in server, client;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(stoi(port));

    int socket_desc, client_sock, read_size, c = sizeof(client);
    const unsigned int MAX_BUF_LENGTH = 1024;
    vector<char> buffer(MAX_BUF_LENGTH);
    string req, resp;

    //Create socket
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1) {
        printf("Could not create socket");
        return -1;
    }

    //Bind
    if (bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Bind failed");
        return -1;
    }

    listen(socket_desc, 100);
    client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t *)&c);
    if (client_sock < 0) {
        perror("Accept failed");
        return -1;
    }

    // Receive a message from client
    while ((read_size = recv(client_sock, &buffer[0], buffer.size(), 0)) > 0) {
        req.append(buffer.cbegin(), buffer.cend());

        // resp = doSomething();
        printf("MESSAGE FROM CLIENT: %s\n", req.c_str());
        write(client_sock, resp.c_str(), resp.length());
    }

    if (read_size == 0) {
        puts("Client disconnected");
        fflush(stdout);
    } else if (read_size == -1) {
        perror("recv failed");
    }

    return 0;
}
