#include "helpers.hpp"

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

int GetPositionInVector(std::vector<std::string> const &vec, std::string word) {
  auto it = std::find(vec.begin(), vec.end(), word);
  int index = std::distance(vec.begin(), it);
  return index;
}

std::vector<std::string> SplitStringToVector(std::string input) {
  std::stringstream ss(input);
  std::istream_iterator<std::string> begin(ss);
  std::istream_iterator<std::string> end;
  std::vector<std::string> commands(begin, end);

  return commands;
}

char **ConvertVectorToCharArray(std::vector<std::string> const &commandList) {
  int size = commandList.size();
  int index = 0;
  char *arg;
  char **args = new char *[size + 1];

  for (auto cmd : commandList) {
    arg = new char[cmd.length()];
    arg = strdup(cmd.c_str());
    args[index] = arg;

    index++;
  }

  // Needs to be NULL terminated
  args[size] = NULL;

  return args;
}

bool IsInteger(std::string input) {
  for (auto c : input) {
    if (!std::isdigit(static_cast<unsigned char>(c))) return false;
  }
  return true;
}