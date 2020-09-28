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
std::set<std::string> native_commands = {"exit",  "jobs",    "kill", "resume",
                                         "sleep", "suspend", "wait"};

bool IsNativeCommand(std::string command) {
  return native_commands.find(command) != native_commands.end();
}

std::vector<std::string> splitCommands(std::string const &user_input) {
  std::stringstream ss(user_input);
  std::istream_iterator<std::string> begin(ss);
  std::istream_iterator<std::string> end;
  std::vector<std::string> commands(begin, end);

  return commands;
}

std::vector<std::string> getUserInput() {
  std::string input;

  std::cout << "SHELL379: ";
  std::getline(std::cin, input);

  return splitCommands(input);
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

// TODO: move to another file
void jobs_command() {
  char *args[2] = {(char *)"ps", NULL};
  if (execvp(args[0], args) < 0) {
    perror("exec error");
    _exit(123);
  }
}

// TODO: move to another file
void exit_command() {
  for (auto entry : process_control_block) {
    std::pair<int, std::string> process = entry;
    int pid = process.first;

    kill(pid, SIGKILL);
  }

  int rc_id;
  wait(&rc_id);
  // TODO: sys time
  _exit(1);
}

// TODO: move to another file
void kill_command(int id) {
  if (kill(id, SIGKILL) < 0) {
    perror("kill failed");
    _exit(1);
  }
}

// TODO: move to another file
void resume_command(int id) {}

// TODO: move to another file
void sleep_command(char **args) { execvp(args[0], args); }

// TODO: move to another file
void suspend_command(int id) {}

// TODO: move to another file
void wait_command(int id) {}

// TODO: move to another file
void runNativeCommands(std::vector<std::string> const &commands) {
  char **args = getBasicArgs(commands);
  // std::cout << "commands: " << args[0] << " " << args[1] << std::endl;

  if (strcmp(args[0], "sleep") == 0) {
    sleep_command(args);
  } else if (strcmp(args[0], "exit") == 0) {
    exit_command();
  } else if (strcmp(args[0], "kill") == 0) {
    kill_command(atoi(args[1]));
  } else if (strcmp(args[0], "jobs") == 0) {
    jobs_command();
  }
}

void runCommands(std::vector<std::string> const &commands) {
  if (strcmp(commands[0].c_str(), "exit") == 0) {
    exit_command();
  } else {
    int pid = fork();
    if (pid < 0) {
      // fork failed
      fprintf(stderr, "fork failed\n");  // TODO: should this be perror?
    } else if (pid == 0) {
      // child process
      printf("INSIDE CHILD PROCESS\n");
      // run routine for native commands
      runNativeCommands(commands);

    } else {
      // parent
      // TODO: PCB entry needs to take in whole command joined together
      process_control_block.insert(
          std::pair<int, std::string>(pid, commands[0]));
      int rc_wait;
      wait(&rc_wait);
      std::cout << "parent process done waiting..." << std::endl;
    }
  }
}

// idk if i need to other signals....
// void sigint_handler(int sig) {}

int main() {
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

    // TODO: eventually for non-native commands will need to validate with
    if (IsNativeCommand(user_input.at(0))) {
      runCommands(user_input);
    } else {
      std::cout << "not native command" << std::endl;
    }
  }
  return 0;
}