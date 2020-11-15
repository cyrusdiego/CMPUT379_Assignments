/*
	C ECHO client example using sockets
*/
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <string>
using namespace std;

bool is_valid_port(string port) {
    int p = stoi(port);
    return p >= 5000 && p <= 64000;
}

// https://man7.org/linux/man-pages/man3/inet_pton.3.html
bool is_valid_ip(string ip) {
    unsigned char buf[sizeof(struct in6_addr)];
    int s = inet_pton(AF_INET, ip.c_str(), buf);

    return s == 1;
}

/**
 * Ensures:
 * - Valid number of input provided
 * - validate if passed arguments are valid ports and ip address
 */
pair<string, string> parse_input(int argc, char **argv) {
    if (argc > 3 || argc <= 1 || !is_valid_port(argv[1]) || !is_valid_ip(argv[2])) {
        perror("Invalid arguments");
        exit(-1);
    }
    return pair<string, string>(argv[1], argv[2]);
}

int main(int argc, char *argv[]) {
    auto args = parse_input(argc, argv);
    string port = args.first;
    string ip = args.second;

    // // code from eClass, modified to accept port and ip address from input
    // int sock;
    // struct sockaddr_in server;
    // char message[1000], server_reply[1000];

    // //Create socket
    // sock = socket(AF_INET, SOCK_STREAM, 0);
    // if (sock == -1) {
    //     printf("Could not create socket");
    // }
    // puts("Socket created");

    // server.sin_addr.s_addr = inet_addr(ip.c_str());
    // server.sin_family = AF_INET;
    // server.sin_port = htons(stoi(port));

    // //Connect to remote server
    // if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
    //     perror("connect failed. Error");
    //     return 1;
    // }

    // puts("Connected\n");

    // //keep communicating with server
    // while (1) {
    //     memset(message, 0, 1000);
    //     printf("Enter message : ");
    //     scanf("%s", message);

    //     //Send some data
    //     puts("Server send :");
    //     puts(message);
    //     if (send(sock, message, strlen(message), 0) < 0) {
    //         puts("Send failed");
    //         return 1;
    //     }

    //     //Receive a reply from the server
    //     memset(server_reply, 0, 1000);
    //     if (recv(sock, server_reply, 1000, 0) < 0) {
    //         puts("recv failed");
    //         break;
    //     }
    //     puts("Server reply: ");
    //     puts(server_reply);
    // }

    // close(sock);
    return 0;
}
