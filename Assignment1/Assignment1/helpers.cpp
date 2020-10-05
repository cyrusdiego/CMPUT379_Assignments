#include "helpers.hpp"

/**
 * converts string native command to enum for easy use in switch
 */
native_commands_enum NativeCommandToEnum(std::string const &arg) {
  static const std::map<std::string, native_commands_enum> command_to_enum{
      {"exit", native_commands_enum::exit_command},
      {"jobs", native_commands_enum::jobs_command},
      {"kill", native_commands_enum::kill_command},
      {"resume", native_commands_enum::resume_command},
      {"sleep", native_commands_enum::sleep_command},
      {"suspend", native_commands_enum::suspend_command},
      {"wait", native_commands_enum::wait_command}};

  return command_to_enum.at(arg);
}

/**
 * get index of element in vector
 */
int GetPositionInVector(std::vector<std::string> const &vec, std::string word) {
  auto it = std::find(vec.begin(), vec.end(), word);
  int index = std::distance(vec.begin(), it);
  return index;
}

/**
 * splits string into a vector
 */
std::vector<std::string> SplitStringToVector(std::string input,
                                             char delimiter) {
  // https://stackoverflow.com/questions/5607589/right-way-to-split-an-stdstring-into-a-vectorstring
  if (delimiter == ' ') {
    std::stringstream ss(input);
    std::istream_iterator<std::string> begin(ss);
    std::istream_iterator<std::string> end;
    std::vector<std::string> commands(begin, end);
    return commands;
  }

  // https://www.fluentcpp.com/2017/04/21/how-to-split-a-string-in-c/
  std::vector<std::string> splitInput;
  std::string word;
  std::istringstream ss1(input);
  while (std::getline(ss1, word, delimiter)) {
    splitInput.push_back(word);
  }

  return splitInput;
}

/**
 * checks if string is an integer
 */
bool IsInteger(std::string input) {
  std::regex e("([0-9]*)");
  return std::regex_match(input, e);
}

/**
 * checks if string is a decimal
 */
bool IsDecimal(std::string input) {
  std::regex e("([0-9]*\\.[0-9]+|[0-9]+)");
  return std::regex_match(input, e);
}