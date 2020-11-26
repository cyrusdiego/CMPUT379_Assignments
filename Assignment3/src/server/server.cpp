
#include <arpa/inet.h>
#include <poll.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "../../include/helpers.hpp"
#include "../../include/tands.hpp"

struct server_stats {
    int transaction_number = 0;
};

server_stats statistics;

int parse_job(std::string job) {
    int num = stoi(job.substr(1, job.length()));

    if (num < 0) {
        return -1;
    }

    return num;
}
/**
 * Ensures:
 * - Valid number of input provided
 * - validate if passed arguments are valid port
 */
std::string parse_input(int argc, char **argv) {
    if (argc > 2 || argc <= 1 || !is_valid_port(argv[1])) {
        perror("Invalid arguments");
        exit(-1);
    }
    return argv[1];
}

int main(int argc, char *argv[]) {
    std::string port = parse_input(argc, argv);
    //TODO: should i close connection on any fail
    // code from eClass, modified to accept port from input
    struct sockaddr_in server, client;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(stoi(port));

    int socket_desc, client_sock, read_size, c = sizeof(client);
    const unsigned int MAX_BUF_LENGTH = 1024;
    std::vector<char> buffer(MAX_BUF_LENGTH);
    std::string req, resp;

    // Create socket
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1) {
        printf("Could not create socket");
        return -1;
    }

    // Bind
    if (bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Bind failed");
        return -1;
    }

    poll_fd.fd = socket_desc;
    poll_fd.events = POLLIN;
    int timeout = 30000;  // 30 sec

    listen(socket_desc, 100);
    client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t *)&c);
    if (client_sock < 0) {
        perror("Accept failed");
        return -1;
    }

    // Receive a message from client
    int n, job_count = 0;
    while ((read_size = recv(client_sock, &buffer[0], buffer.size(), 0)) > 0) {
        req.append(buffer.cbegin(), buffer.cend());

        // Do work
        if ((n = parse_job(req)) < 0) {
            continue;
        }
        Trans(n);
        statistics.transaction_number++;

        // Send response back to client
        std::stringstream resp;
        resp << "D" << ++job_count;
        write(client_sock, resp.str().c_str(), resp.str().length());
    }

    if (read_size == 0) {
        puts("Client disconnected");
        fflush(stdout);
    } else if (read_size == -1) {
        perror("recv failed");
    }

    return 0;
}
