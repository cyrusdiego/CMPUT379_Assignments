#include "../include/helpers.hpp"
bool is_valid_port(std::string port) {
    int p = stoi(port);
    return p >= 5000 && p <= 64000;
}

// https://man7.org/linux/man-pages/man3/inet_pton.3.html
bool is_valid_ip(std::string ip) {
    unsigned char buf[sizeof(struct in6_addr)];
    int s = inet_pton(AF_INET, ip.c_str(), buf);

    return s == 1;
}

/**
 * Parse T<n> to a job type and time
 */
std::pair<char, int> parse_job(std::string job) {
    char type = job.front();
    std::string s(job.begin() + 1, job.end());
    long int time = stoll(s);

    return std::pair<char, int>(type, time);
}