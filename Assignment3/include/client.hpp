#ifndef CLIENT_HPP
#define CLIENT_HPP
#include <sys/socket.h>
#include <unistd.h>

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
    int CreateSocket();

   private:
    client_statistics stats;
    std::vector<char> buffer;
    sockaddr_in server_socket;
    int sock;
    Logger logger;
};

#endif