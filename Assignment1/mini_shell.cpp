#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#define LINE_LENGTH 100    // Max # of characters in an input line
#define MAX_ARGS 7         // Max number of arguments to a command
#define MAX_LENGTH 20      // Max # of characters in an argument
#define MAX_PT_ENTRIES 32  // Max entries in the Process Table

const std::string RUNNING_PROCESS = "Running processes:\n";
const std::string PROCESSES = "Processes = ";
const std::string COMPLETED_PROCESSES = "Completed processes:\n";
const std::string USER_TIME = "User time = ";
const std::string SYS_TIME = "Sys time = ";
const std::string PCB_COLUMNDS = "#    PID S SEC COMMAND\n";

std::map<int, std::string> process_control_block;
const std::set<std::string> native_commands = {
    "exit", "jobs", "kill", "resume", "sleep", "suspend", "wait"};

enum native_commands_enum {
  exit_command,
  jobs_command,
  kill_command,
  resume_command,
  sleep_command,
  suspend_command,
  wait_command
};

// std::vector<std::string> splitCommands(std::string const &user_input) {
//   std::stringstream ss(user_input);
//   std::istream_iterator<std::string> begin(ss);
//   std::istream_iterator<std::string> end;
//   std::vector<std::string> commands(begin, end);

//   return commands;
// }

// TODO: move to another file
void JobsCommand() {
  char *args[2] = {(char *)"ps", NULL};
  if (execvp(args[0], args) < 0) {
    perror("exec error");
    _exit(123);
  }
}

// TODO: move to another file
void ExitCommand() {
  for (auto entry : process_control_block) {
    std::pair<int, std::string> process = entry;
    int pid = process.first;

    kill(pid, SIGKILL);
  }

  int rc_id;
  // TODO: change to wait_pid
  wait(&rc_id);
  // TODO: sys time
  _exit(1);
}

// TODO: move to another file
void KillCommand(int id) {
  if (kill(id, SIGKILL) < 0) {
    perror("kill failed");
    _exit(1);
  }
}

// TODO: move to another file
void ResumeCommand(int id) {}

// TODO: move to another file
// TODO: this method may be applicable to multiple paths...
void SleepCommand(char **args) { execvp(args[0], args); }

// TODO: move to another file
void SuspendCommand(int id) {}

// TODO: move to another file
void WaitCommand(int id) {  // change to wait_pid
  wait(&id);
}

native_commands_enum ArgToEnum(std::string arg) {
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

void runNativeUserCommand(std::vector<std::string> const &commands) {
  std::string arg1 = commands[0];

  // TODO: idk if these native user commands will ever have more than 2 args...
  char **args = getBasicArgs(commands);
  switch (ArgToEnum(arg1)) {
    case native_commands_enum::exit_command: {
      printf("<------ RUNNING EXIT ------>");
      ExitCommand();
      break;
    }
    case native_commands_enum::sleep_command: {
      printf("<------ RUNNING SLEEP ------>");
      SleepCommand(args[1]);
      break;
    }
    case native_commands_enum::jobs_command: {
      printf("<------ RUNNING JOBS ------>");
      JobsCommand();
      break;
    }
    case native_commands_enum::kill_command: {
      printf("<------ RUNNING KILL ------>");
      KillCommand(atoi(args[1]));
      break;
    }
    case native_commands_enum::resume_command: {
      // ResumeCommand();
      break;
    }
    case native_commands_enum::suspend_command: {
      // ResumeCommand();
      break;
    }
    case native_commands_enum::wait_command: {
      // WaitCommand();
      break;
    }
  }
}

bool isNativeCommand(std::string command) {
  return native_commands.find(command) != native_commands.end();
}

std::vector<std::string> getUserInput() {
  std::string input;

  std::cout << "SHELL379: ";
  std::getline(std::cin, input);
  std::stringstream ss(input);
  std::istream_iterator<std::string> begin(ss);
  std::istream_iterator<std::string> end;
  std::vector<std::string> commands(begin, end);

  return commands;
}

char **getBasicArgs(std::vector<std::string> const &commandList) {
  // TODO: generalize so that args size is variable
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

// idk if i need to other signals....
// void sigint_handler(int sig) {}

int main() {
  // TODO: idk where to put this...
  struct sigaction sa;
  sa.sa_handler = SIG_IGN;
  sa.sa_flags = 0;
  sa.sa_mask = 0;
  if (sigaction(SIGINT, &sa, NULL) == -1) {
    perror("sigaction");
    exit(1);
  }

  while (1) {
    std::vector<std::string> user_input = getUserInput();
    if (user_input.size() <= 0) {
      continue;
    }
    // TODO: eventually for non-native commands will need to validate with
    if (isNativeCommand(user_input.at(0))) {
      runNativeUserCommand(user_input);
    } else {
      std::cout << "not native command" << std::endl;
    }
  }
  return 0;
}