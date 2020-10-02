#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "constants.hpp"

// TODO: idk how to best organize helpers (cpp + header?? )
namespace helpers {
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

// TODO: generalize so that args size is variable
char **ConvertVectorToCharArray(std::vector<std::string> const &commandList) {
  if (commandList.size() == 1) {
    char *arg1 = new char[commandList[0].length() + 1];
    char **args = new char *[2];
    args[0] = strdup(commandList[0].c_str());
    args[1] = NULL;
    return args;
  }
  char *arg1 = new char[commandList[0].length() + 1];
  char *arg2 = new char[commandList[1].length() + 1];
  char **args = new char *[3];
  args[0] = strdup(commandList[0].c_str());
  args[1] = strdup(commandList[1].c_str());
  args[2] = NULL;

  return args;
}
}  // namespace helpers
