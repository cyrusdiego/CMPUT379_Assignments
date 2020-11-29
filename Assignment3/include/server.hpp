#ifndef CLIENT_HPP
#define CLIENT_HPP

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

#include <chrono>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "helpers.hpp"
#include "tands.hpp"

const unsigned int MAX_BUF_LENGTH = 1024;
const unsigned int MAX_CONNECTIONS = 100;

struct server_stats {
    std::unordered_map<std::string, int> transaction_numbers;
    int job_count = 0;
    std::chrono::duration<double> server_duration;
};

class Server {
   public:
    Server(std::string port);
    ~Server();
    int Setup();
    int Run();
    void Cleanup();
    void LogJob(int job, std::string client_name);
    void UpdateStats(std::string machine);

   private:
    server_stats stats;
    struct sockaddr_in server;
    char buffer[MAX_BUF_LENGTH];
    struct pollfd fds[MAX_CONNECTIONS];
    int server_fd, client_sd, rc, on = 1, num_fds = 1;
    const int TIMEOUT = 30000;
    bool run_server = true, compress_array = false;
};

#endif