#include "../../include/server.hpp"

Server::Server(std::string port) : logger() {
    // Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(stoi(port));
}

int Server::setup() {
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
    memset(buffer, 0, MAX_BUF_LENGTH);
    // Setup listenting socket
    fds[0].fd = server_fd;
    fds[0].events = POLLIN;

    return 0;
}

// Referenced:
// https://www.ibm.com/support/knowledgecenter/ssw_ibm_i_71/rzab6/poll.htm
int Server::run() {
    do {
        rc = poll(fds, num_fds, TIMEOUT);
        if (rc < 0) {
            perror("Polling failed");
            return -1;
        }
        if (rc == 0) {
            printf("TIMED OUT!");
            return 0;
        }

        int current_size = num_fds;
        for (int i = 0; i < current_size; i++) {
            // if (fds[i].revents == 0) continue;
            bool connected = true;

            if (fds[i].revents == POLLERR || fds[i].revents == POLLNVAL) {
                printf("Error, unexpected result: %d\n", fds[i].revents);
                run_server = false;
                break;
            } else if (fds[i].revents == POLLHUP) {
                close(fds[i].fd);
                fds[i].fd = -1;
                compress_array = true;
                connected = false;
            }

            if (connected) {
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
                        rc = recv(fds[i].fd, buffer, MAX_BUF_LENGTH, 0);
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

                        auto job = parse_job(buffer);
                        if (job.second < 0) {
                            continue;
                        }

                        Trans(job.second);
                        statistics.transaction_number++;
                        std::string s = "D" + std::to_string(++statistics.job_count);
                        memset(buffer, 0, MAX_BUF_LENGTH);
                        strcpy(buffer, s.c_str());
                        buffer[s.length()] = '\0';

                        rc = send(fds[i].fd, buffer, s.length() + 1, 0);
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

    return 0;
}

void Server::cleanup() {
    for (int i = 0; i < num_fds; i++) {
        if (fds[i].fd >= 0) {
            close(fds[i].fd);
        }
    }
}
