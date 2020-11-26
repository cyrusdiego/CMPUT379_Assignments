/*
	C ECHO client example using sockets
*/
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>
#include <string>
#include <vector>

#include "../../include/tands.hpp"

struct client_statistics {
    int transaction_count = 0;
    int sleep_count = 0;
};

bool is_valid_port(std::string port) {
    int p = stoi(port);
    return p >= 5000 && p <= 64000;
}

// https://man7.org/linux/man-pages/man3/inet_pton.3.html
bool is_valid_ip(std::string ip) {
    unsigned char buf[sizeof(struct in6_addr)];
    int s = inet_pton(AF_INET, ip.c_str(), buf);

    return s == 1;
}

/**
 * Parse T<n> to a job type and time
 */
std::pair<char, int> parse_job(std::string job) {
    char type = job.front();
    std::string s(job.begin() + 1, job.end());
    long int time = stoll(s);

    return std::pair<char, int>(type, time);
}

/**
 * Ensures:
 * - Valid number of input provided
 * - validate if passed arguments are valid ports and ip address
 */
std::pair<std::string, std::string> parse_input(int argc, char **argv) {
    if (argc > 3 || argc <= 1 || !is_valid_port(argv[1]) || !is_valid_ip(argv[2])) {
        perror("Invalid arguments");
        exit(-1);
    }
    return std::pair<std::string, std::string>(argv[1], argv[2]);
}

int main(int argc, char *argv[]) {
    auto args = parse_input(argc, argv);
    std::string port = args.first;
    std::string ip = args.second;

    // code from eClass, modified to accept port and ip address from input
    int sock;
    struct sockaddr_in server;
    server.sin_addr.s_addr = inet_addr(ip.c_str());
    server.sin_family = AF_INET;
    server.sin_port = htons(stoi(port));

    const unsigned int MAX_BUF_LENGTH = 1024;
    std::string req, resp;
    std::vector<char> buffer(MAX_BUF_LENGTH);

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Could not create socket");
        return -1;
    }

    // Main client loop
    while (std::getline(std::cin, req)) {
        auto job = parse_job(req);

        if (job.first == 'T') {
            // Connect to server
            if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
                perror("Connect failed");
                return -1;
            }

            // Send to server
            if (send(sock, req.c_str(), req.length(), 0) < 0) {
                printf("Send failed");
                return -1;
            }

            // Receive a reply from the server
            if (recv(sock, &buffer[0], buffer.size(), 0) < 0) {
                printf("Recieve failed");
                return -1;
            }

            resp.append(buffer.cbegin(), buffer.cend());
            // doSomething(resp);
            printf("MESSAGE FROM SERVER: %s\n", resp.c_str());
            close(sock);
        } else {
            Sleep(job.second);
        }
    }

    return 0;
}
