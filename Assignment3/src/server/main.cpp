
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
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
    int job_count = 0;

    bool run_server = true, compress_array = false;
    int server_fd, client_sd, rc, on = 1, timeout = 30000;
    struct pollfd fds[100];  // max 100 connections
    int num_fds = 1;         // number of active connections
    // Message passing structures
    const unsigned int MAX_BUF_LENGTH = 1024;
    std::vector<char> buffer(MAX_BUF_LENGTH);
    std::stringstream resp;
    std::string req;

    // Prepare the sockaddr_in structure
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(stoi(port));

    // Create socket and store socket descriptor
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Could not create socket");
        return -1;
    }

    // Make socket descriptor reusable
    rc = setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));
    if (rc < 0) {
        perror("Could not make socket descriptor reusable");
        close(server_fd);
        return -1;
    }

    // Make socket descriptor non blocking
    rc = ioctl(server_fd, FIONBIO, (char *)&on);
    if (rc < 0) {
        perror("Could not make socket nonblocking");
        close(server_fd);
        return -1;
    }

    // Bind socket
    rc = bind(server_fd, (struct sockaddr *)&server, sizeof(server));
    if (rc < 0) {
        perror("Could not bind socket");
        close(server_fd);
        return -1;
    }

    // Listen (second param is max number of connections)
    rc = listen(server_fd, 100);
    if (rc < 0) {
        perror("Could not listen to socket");
        close(server_fd);
        return -1;
    }

    memset(fds, 0, sizeof(fds));
    // Setup listenting socket
    fds[0].fd = server_fd;
    fds[0].events = POLLIN;

    do {
        rc = poll(fds, num_fds, timeout);
        if (rc < 0) {
            perror("Polling failed");
            return -1;
        }
        if (rc == 0) {
            printf("TIMEDOUT!");
            return 0;
        }

        int current_size = num_fds;
        for (int i = 0; i < current_size; i++) {
            if (fds[i].revents == 0) continue;

            if (fds[i].revents != POLLIN) {
                std::cout << fds[i].fd << " " << server_fd;
                printf("Error, unexpected result: %d\n", fds[i].revents);
                run_server = false;
                break;
            }

            // This is the server socket
            if (fds[i].fd == server_fd) {
                do {
                    client_sd = accept(server_fd, NULL, NULL);
                    if (client_sd < 0) {
                        if (errno != EWOULDBLOCK) {
                            perror("Accept failed");
                            run_server = false;
                        }
                        break;
                    }

                    printf("New connection %d\n", client_sd);
                    fds[num_fds].fd = client_sd;
                    fds[num_fds].events = POLLIN;
                    num_fds++;
                } while (client_sd != -1);
            } else {  // this is a client fd

                bool close_conn = false;

                do {
                    rc = recv(fds[i].fd, &buffer[0], MAX_BUF_LENGTH, 0);
                    if (rc < 0) {
                        if (errno != EWOULDBLOCK) {
                            perror("Recieve failed");
                            close_conn = true;
                        }
                        break;
                    }

                    if (rc == 0) {
                        printf("Connection closed\n");
                        close_conn = true;
                        break;
                    }

                    req.append(buffer.cbegin(), buffer.cend());
                    auto job = parse_job(req);
                    if (job.second < 0) {
                        continue;
                    }
                    Trans(job.second);
                    statistics.transaction_number++;

                    resp << "D" << ++job_count;
                    std::string s = resp.str();
                    rc = send(fds[i].fd, s.c_str(), s.length(), 0);
                    if (rc < 0) {
                        perror("Send failed");
                        close_conn = true;
                        break;
                    }
                } while (1);

                if (close_conn) {
                    close(fds[i].fd);
                    fds[i].fd = -1;
                    compress_array = true;
                }
            }
        }

        // To prevent fragmented fds we push empty to the back
        if (compress_array) {
            compress_array = false;
            for (int i = 0; i < num_fds; i++) {
                if (fds[i].fd == -1) {
                    for (int j = i; j < num_fds; j++) {
                        fds[j].fd = fds[j + 1].fd;
                    }
                    i--;
                    num_fds--;
                }
            }
        }
    } while (run_server);

    for (int i = 0; i < num_fds; i++) {
        if (fds[i].fd >= 0) {
            close(fds[i].fd);
        }
    }

    return 0;
}
