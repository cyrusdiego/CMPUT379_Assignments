#include "../../include/server.hpp"

Server::Server(std::string port) {
    // Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(stoi(port));
    std::cout << "Using port " << port << std::endl;
}

Server::~Server() {
    std::stringstream stream;
    stream << "\nSUMMARY" << std::endl;
    for (auto entry : stats.transaction_numbers) {
        stream << std::right << std::setw(4) << entry.second;
        stream << " from " << entry.first << std::endl;
    }

    std::chrono::duration<double> time = stats.end - stats.start;
    double transactions_per_sec = stats.job_count / time.count();

    stream << std::right << std::setprecision(2) << transactions_per_sec;
    stream << " transaction/sec  (" << stats.job_count << "/" << std::setprecision(2) << time.count() << ")" << std::endl;
    std::cout << stream.str() << std::endl;

    close(server_fd);
}

int Server::Setup() {
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
int Server::Run() {
    bool timer_started = false;
    do {
        rc = poll(fds, num_fds, TIMEOUT);
        if (rc < 0) {
            perror("Polling failed");
            return -1;
        }
        if (rc == 0) {
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

                        fds[num_fds].fd = client_sd;
                        fds[num_fds].events = POLLIN;
                        num_fds++;
                    } while (client_sd != -1);
                } else {  // this is a client fd

                    // Check if there's any changes to the fd
                    rc = poll(&fds[i], 1, 100);
                    if (rc < 0) {
                        perror("Poll failed");
                        return -1;
                    }
                    if (rc == 0) {
                        continue;
                    }

                    bool close_conn = false;
                    rc = recv(fds[i].fd, buffer, MAX_BUF_LENGTH, 0);
                    if (rc < 0) {
                        if (errno != EWOULDBLOCK) {
                            perror("Recieve failed");
                        }
                        close_conn = true;
                    }

                    if (rc == 0) {
                        close_conn = true;
                    }

                    if (!close_conn) {
                        if (!timer_started) {
                            stats.start = std::chrono::system_clock::now();
                            timer_started = true;
                        }
                        // Parse client message
                        std::string req = std::string(buffer);
                        int del = req.find(' ');
                        std::string transaction = req.substr(0, del);
                        std::string machine = req.substr(del + 1, std::string::npos);
                        auto job = parse_job(transaction);

                        if (job.second < 0) {
                            continue;
                        }

                        // Log message
                        LogJob(job.second, machine);

                        // Do work
                        Trans(job.second);

                        // Update stats and log complete
                        UpdateStats(machine);
                        LogJob(-1, machine);
                        stats.end = std::chrono::system_clock::now();

                        // Build message to client
                        std::string s = "D" + std::to_string(++stats.job_count);
                        memset(buffer, 0, MAX_BUF_LENGTH);
                        strcpy(buffer, s.c_str());
                        buffer[s.length()] = '\0';

                        // Send message to client
                        rc = send(fds[i].fd, buffer, s.length() + 1, 0);
                        if (rc < 0) {
                            perror("Send failed");
                            close_conn = true;
                        }

                        memset(buffer, 0, MAX_BUF_LENGTH);
                    } else {
                        close(fds[i].fd);
                        fds[i].fd = -1;
                        compress_array = true;
                    }
                }
            }
        }

        // To prevent fragmented fds we push empty to the back
        // TODO: extract to function?
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

void Server::LogJob(int job, std::string client_name) {
    // https://stackoverflow.com/questions/40705817/c-chrono-get-seconds-with-a-precision-of-3-decimal-places
    const auto current_time = std::chrono::system_clock::now();
    double time = std::chrono::duration<double>(current_time.time_since_epoch()).count();

    std::stringstream stream;
    if (job != -1) {
        stream << "# ";
        stream << std::right << std::setw(3) << std::to_string(stats.job_count) << " ";
        stream << "(T" << std::right << std::setw(3) << std::to_string(job) << ") ";
        stream << "from " << client_name;
    } else {
        stream << "# ";
        stream << std::right << std::setw(3) << std::to_string(stats.job_count) << " ";
        stream << std::setw(5) << "(Done) from " << client_name;
    }

    std::cout << std::fixed << std::setprecision(2) << time << ": " << stream.str() << std::endl;
    std::cout << std::flush;
    fflush(stdout);
}

void Server::UpdateStats(std::string machine) {
    if (stats.transaction_numbers.find(machine) == stats.transaction_numbers.end()) {
        std::pair<std::string, int> new_entry(machine, 1);
        stats.transaction_numbers.insert(new_entry);
    } else {
        stats.transaction_numbers.at(machine)++;
    }
}

void Server::Cleanup() {
    for (int i = 0; i < num_fds; i++) {
        if (fds[i].fd >= 0) {
            close(fds[i].fd);
        }
    }
}
