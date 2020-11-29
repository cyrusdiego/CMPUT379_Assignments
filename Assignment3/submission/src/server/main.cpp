#include "../../include/server.hpp"

/**
 * Ensures:
 * - Valid number of input provided
 * - validate if passed arguments are valid port
 */
std::string parse_input(int argc, char **argv) {
    if (argc > 2 || argc <= 1 || !is_valid_port(argv[1])) {
        perror("Invalid arguments");
        exit(-1);
    }
    return argv[1];
}

int main(int argc, char *argv[]) {
    std::string port = parse_input(argc, argv);
    Server server(port);
    if (server.Setup() < 0) {
        return -1;
    }
    if (server.Run() < 0) {
        server.Cleanup();
        return -1;
    }
    server.Cleanup();
    return 0;
}
