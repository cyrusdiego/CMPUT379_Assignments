#ifndef HELPERS_H
#define HELPERS_H
#include <arpa/inet.h>

#include <string>

bool is_valid_port(std::string port);

bool is_valid_ip(std::string ip);

std::pair<char, int> parse_job(std::string job);

#endif