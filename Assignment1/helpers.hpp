#ifndef HELPERS_H
#define HELPERS_H
#include <cctype>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "constants.hpp"

native_commands_enum NativeCommandToEnum(std::string const &arg);
int GetPositionInVector(std::vector<std::string> const &vec, std::string word);
std::vector<std::string> SplitStringToVector(std::string input);
char **ConvertVectorToCharArray(std::vector<std::string> const &commandList);
bool IsInteger(std::string input);
#endif