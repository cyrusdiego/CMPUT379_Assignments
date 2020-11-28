/*
	C ECHO client example using sockets
*/
#include "../../include/client.hpp"

Client::Client(std::string port, std::string ip) : logger() {
    server_socket.sin_addr.s_addr = inet_addr(ip.c_str());
    server_socket.sin_family = AF_INET;
    server_socket.sin_port = htons(stoi(port));

    pid = getpid();
    gethostname(machine_name, _POSIX_HOST_NAME_MAX);

    std::stringstream stream;
    stream << machine_name << "." << pid;
    logger.SetTitle(stream.str());
    logger.HasInCol(true);

    stream.str(std::string(""));
    stream << "Using port " << port << std::endl
           << "Using server address " << ip << std::endl
           << "Host " << machine_name << "." << pid << std::endl;
    logger.Log(stream.str());

    memset(resp, 0, 1024);
}

Client::~Client() {}

int Client::Send(std::string req) {
    // Send to server
    if (send(sock, req.c_str(), req.length(), 0) < 0) {
        printf("Send failed");
        return -1;
    }
    LogJob(req);

    // Receive a reply from the server
    memset(resp, 0, MAX_BUF_LENGTH);
    if (recv(sock, resp, MAX_BUF_LENGTH, 0) < 0) {
        printf("Recieve failed");
        return -1;
    }
    LogJob(resp);

    return 0;
}

int Client::CreateAndConnectToSocket() {
    sock = socket(AF_INET, SOCK_STREAM, 0);
    // Connect to socket
    if (connect(sock, (struct sockaddr *)&server_socket, sizeof(server_socket)) < 0) {
        perror("Connect failed");
        return -1;
    }
    return sock;
}

void Client::LogJob(std::string job) {
    // https://stackoverflow.com/questions/40705817/c-chrono-get-seconds-with-a-precision-of-3-decimal-places
    const auto current_time = std::chrono::system_clock::now();
    double time = std::chrono::duration<double>(current_time.time_since_epoch()).count();

    std::stringstream stream;
    char type = job[0];
    if (type == 'T' || type == 'D') {
        stream << "Send (" << type;
        stream << std::fixed << std::right << std::setw(3) << job.substr(1, std::string::npos);
        stream << ")" << std::endl;
        logger.Log(time, stream.str(), job);
    } else if (type == 'S') {
        stream << "Sleep " << job.substr(1, std::string::npos) << " units" << std::endl;
        logger.Log(stream.str(), job);
    }
    // any other type is not possible, but if it does happen, don't log
}
