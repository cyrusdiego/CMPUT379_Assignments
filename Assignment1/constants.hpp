#ifndef CONSTANTS_H
#define CONSTANTS_H
#include <set>
#include <string>

#define LINE_LENGTH 100   // Max # of characters in an input line
#define MAX_ARGS 7        // Max number of arguments to a command
#define MAX_LENGTH 20     // Max # of characters in an argument
#define MAX_PT_ENTRIES 1  // Max entries in the Process Table

const std::string INVALID_ARGUMENTS = "Invalid arguments\n";
const std::string OPEN_FAILED = "failed to open file\n";
const std::string KILL_FAILED = "kill signal failed\n";
const std::string FORK_FAILED = "fork failed\n";
const std::string EXEC_FAILED = "child did not exit properly\n";
const std::string RUNNING_PROCESS = "Running processes:\n";
const std::string PROCESSES = "Processes =    ";
const std::string COMPLETED_PROCESSES = "Completed processes:\n";
const std::string USER_TIME = "User time =    ";
const std::string SYS_TIME = "Sys time =     ";
const std::string PCB_COLUMNS = "#   PID S SEC COMMAND\n";
const std::string ACTIVE = " active\n";
const std::string SECONDS = " seconds\n";
const std::string SHELL379 = "SHELL379: ";
const std::string RESOURCES_USED = "Resources used:\n";
const std::string PS_COMMAND = "ps -o pid,state,time";
const std::string PS_ERROR = "Could not run 'ps' command :( \n";
const std::string TOO_MANY_BACKGROUND_PROCESSES =
    "Too many background processes\n";

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

#endif