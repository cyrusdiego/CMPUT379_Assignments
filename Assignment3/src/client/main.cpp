#include <iostream>
#include <string>

#include "../../include/client.hpp"
#include "../../include/helpers.hpp"

/**
 * Ensures:
 * - Valid number of input provided
 * - validate if passed arguments are valid ports and ip address
 */
std::pair<std::string, std::string> parse_input(int argc, char **argv) {
    if (argc > 3 || argc <= 1 || !is_valid_port(argv[1]) || !is_valid_ip(argv[2])) {
        perror("Invalid arguments");
        exit(-1);
    }
    return std::pair<std::string, std::string>(argv[1], argv[2]);
}

int main(int argc, char *argv[]) {
    auto args = parse_input(argc, argv);
    std::string port = args.first;
    std::string ip = args.second;
    std::string req;

    Client client(port, ip);
    if (client.CreateAndConnectToSocket() < 0) {
        return -1;
    }

    while (std::getline(std::cin, req)) {
        auto job = parse_job(req);

        if (job.first == 'T') {
            if (client.Send(req) < 0) {
                return -1;
            }
        } else {
            Sleep(job.second);
        }
    }
    return 0;
}
