/*
	C ECHO client example using sockets
*/
#include "../../include/client.hpp"

Client::Client(std::string port, std::string ip) : buffer(MAX_BUF_LENGTH), logger("") {
    server_socket.sin_addr.s_addr = inet_addr(ip.c_str());
    server_socket.sin_family = AF_INET;
    server_socket.sin_port = htons(stoi(port));
    pid = getpid();
    gethostname(machine_name, _POSIX_HOST_NAME_MAX);
    std::cout << machine_name << std::endl;
}

int Client::Send(std::string req) {
    std::string resp;

    // Connect to socket
    if (connect(sock, (struct sockaddr *)&server_socket, sizeof(server_socket)) < 0) {
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
    return 0;
}

int Client::CreateSocket() {
    sock = socket(AF_INET, SOCK_STREAM, 0);
    return sock;
}
