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

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "helpers.hpp"
#include "logger.hpp"
#include "tands.hpp"

const unsigned int MAX_BUF_LENGTH = 1024;
const unsigned int MAX_CONNECTIONS = 100;

struct server_stats {
    int transaction_number = 0;
    int job_count = 0;
};

class Server {
   public:
    Server(std::string port);
    int setup();
    int run();
    void cleanup();

   private:
    Logger logger;
    server_stats statistics;
    struct sockaddr_in server;
    char buffer[MAX_BUF_LENGTH];
    struct pollfd fds[MAX_CONNECTIONS];
    int server_fd, client_sd, rc, on = 1, num_fds = 1;
    const int TIMEOUT = 30000;
    bool run_server = true, compress_array = false;
};

#endif