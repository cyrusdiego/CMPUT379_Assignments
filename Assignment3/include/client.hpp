#ifndef CLIENT_HPP
#define CLIENT_HPP
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "helpers.hpp"
#include "logger.hpp"
#include "tands.hpp"

const unsigned int MAX_BUF_LENGTH = 1024;

struct client_statistics {
    int transaction_count = 0;
    int sleep_count = 0;
};

class Client {
   public:
    Client(std::string port, std::string ip);
    ~Client();
    int Send(std::string job);
    int CreateAndConnectToSocket();
    void LogJob(std::string job);

   private:
    client_statistics stats;
    char resp[MAX_BUF_LENGTH], machine_name[_POSIX_HOST_NAME_MAX];
    sockaddr_in server_socket;
    int sock, pid;
    Logger logger;
};

#endif