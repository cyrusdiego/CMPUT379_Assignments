#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <fstream>
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

struct ioDetails {
  std::string fileIN;
  int indexIN;
  std::string fileOUT;
  int indexOUT;
};

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

native_commands_enum ArgToEnum(std::string const &arg) {
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

char **GetBasicArgs(std::vector<std::string> const &commandList) {
  // TODO: generalize so that args size is variable
  if (commandList.size() == 1) {
    char *arg1 = new char[commandList[0].length() + 1];
    char **args = new char *[2];
    args[0] = strdup(commandList[0].c_str());
    args[1] = NULL;
    return args;
  }

  std::cout << "creating args as char**" << std::endl
            << commandList[0] << std::endl;
  char *arg1 = new char[commandList[0].length() + 1];
  char *arg2 = new char[commandList[1].length() + 1];
  char **args = new char *[3];
  args[0] = strdup(commandList[0].c_str());
  args[1] = strdup(commandList[1].c_str());
  args[2] = NULL;

  return args;
}

std::vector<std::string> SplitString(std::string input) {
  std::stringstream ss(input);
  std::istream_iterator<std::string> begin(ss);
  std::istream_iterator<std::string> end;
  std::vector<std::string> commands(begin, end);

  return commands;
}
std::string GetFileContents(std::string fileName) {
  std::ifstream fstream(fileName, std::ifstream::in);
  std::ostringstream ss;

  ss << fstream.rdbuf();
  fstream.close();
  return ss.str();
}

ioDetails GetIOArgs(std::vector<std::string> const &command) {
  std::string fileIN = "";
  std::string fileOUT = "";
  int indexIN = -1;
  int indexOUT = -1;
  int counter = 0;
  for (auto cmd : command) {
    for (auto letter : cmd) {
      if (letter == '<') {
        indexIN = counter;
        fileIN = cmd;
        fileIN.erase(0, 1);
        break;
      }

      if (letter == '>') {
        indexOUT = counter;
        fileOUT = cmd;
        fileOUT.erase(0, 1);
        break;
      }
    }
    counter++;
  }
  ioDetails details;
  details.fileIN = fileIN;
  details.indexIN = indexIN;
  details.fileOUT = fileOUT;
  details.indexOUT = indexOUT;

  return details;
}

// std::string FormatTimeToSeconds(std::string process_time) {
//   // [[DD-]hh:]mm:ss
//   size_t position = 0;
//   std::string substring;
//   std::string delimeter = ":";
//   std::vector<std::string> extracted_time;
//   int64_t time;
//   while ((position = process_time.find(delimeter)) != std::string::npos) {
//     substring = process_time.substr(0, position);
//     extracted_time.push_back(substring);
//     process_time.erase(0, position + delimeter.length());
//   }

//   for (auto t : extracted_time) {
//     std::cout << t << std::endl;
//     // time += atoi(t.c_str());
//   }

//   return std::to_string(time);
// }

// TODO: move to another file
void JobsCommand() {
  std::vector<std::string> ps_table;
  std::vector<std::string> jobs_table;
  std::string ps_results;
  std::string jobs_row;
  std::string process_times;
  std::ostringstream ss;
  int active_processes = 0;
  int shell_379_processes = 0;
  time_t user_time;
  time_t sys_time;
  struct rusage usage;
  FILE *p;
  int c;

  if ((p = popen(PS_COMMAND.c_str(), "r")) == NULL) {
    printf("%s", PS_ERROR.c_str());
  } else {
    // read output of ps into a string
    while ((c = fgetc(p)) != EOF) {
      ps_results.push_back(c);
    }
    // close file
    pclose(p);

    // split results from ps comand into vector
    ps_table = SplitString(ps_results);
    for (int i = 3; i < ps_table.size(); i += 6) {
      // get each column entry: pid, state, time
      int pid = stoi(ps_table[i]);
      std::string state = ps_table[i + 1];
      std::string time = ps_table[i + 2];
      // TODO: format time string
      // std::cout << time << std::endl;
      // std::string time1 = FormatTimeToSeconds(time);
      // std::cout << time << std::endl;

      // only look at processes started by shell379
      std::cout << "from ps: " << pid << std::endl;
      if (process_control_block.count(pid) == 1) {
        std::cout << "PCB has: " << pid << std::endl << std::endl;
        // look for active processes
        if (state == "R") {  // TODO: need to just check if R is in there
          active_processes++;
        }

        // build PCB row entry
        ss << shell_379_processes << ": " << pid << " " << state << " " << time
           << " " << process_control_block.at(pid);
        jobs_row = ss.str();

        ss.str("");
        ss.clear();

        // store row into table
        jobs_table.push_back(jobs_row);
        shell_379_processes++;
      }
    }

    // TODO: VERIFY THIS WORKS
    getrusage(RUSAGE_CHILDREN, &usage);
    user_time = usage.ru_utime.tv_sec;
    sys_time = usage.ru_stime.tv_sec;

    // build Jobs table
    ss << RUNNING_PROCESS << PCB_COLUMNS;
    for (int i = 0; i < jobs_table.size(); i++) {
      ss << jobs_table[i] << "\n";
    }
    ss << PROCESSES << active_processes << ACTIVE << COMPLETED_PROCESSES
       << USER_TIME << user_time << SECONDS << SYS_TIME << sys_time << SECONDS;

    // print jobs table
    std::cout << ss.str();
    for (auto x : process_control_block) {
      std::cout << "PCB entry: " << x.first << " " << x.second;
    }
  }
}

// TODO: move to another file
void ExitCommand() {
  std::cout << "size of PCB: " << process_control_block.size() << std::endl;
  for (auto entry : process_control_block) {
    std::pair<int, std::string> process = entry;
    int pid = process.first;
    std::cout << "kill processes " << pid << std::endl;

    kill(pid, SIGKILL);
    int status;
    waitpid(pid, &status, 0);
  }

  // TODO: VERIFY IF THIS WORKS...
  struct rusage usage;
  getrusage(RUSAGE_CHILDREN, &usage);
  time_t user_time = usage.ru_utime.tv_sec;
  time_t sys_time = usage.ru_stime.tv_sec;
  std::ostringstream ss;

  ss << RESOURCES_USED << USER_TIME << user_time << std::endl
     << SYS_TIME << sys_time << std::endl;

  std::cout << ss.str();
  _exit(1);
}

// TODO: move to another file
void KillCommand(int id) {
  if (kill(id, SIGKILL) < 0) {
    perror("kill failed");
    _exit(1);
  }
  int status;
  waitpid(id, &status, 0);
}

// TODO: move to another file
void ResumeCommand(int id) {
  if (kill(id, SIGCONT) < 0) {
    perror("kill failed");
    _exit(1);
  }
  int status;
  waitpid(id, &status, 0);
}

// TODO: move to another file
void SuspendCommand(int id) {
  if (kill(id, SIGSTOP) < 0) {
    perror("kill failed");
    _exit(1);
  }
  int status;
  waitpid(id, &status, 0);
}

// TODO: move to another file
void WaitCommand(int id) {
  int status;
  waitpid(id, &status, 0);
}

void SleepCommand(int seconds) { sleep(seconds); }

// TODO: WHY IS SHELL379 DOUBLED????
void RunBackgroundProcess(char **args) {
  std::ostringstream ss1;
  for (int i = 0; args[i] != NULL; ++i) {
    ss1 << args[i] << " ";
  }
  std::cout << "\n RUNNING BACKGROUND PROCESS \n" << ss1.str() << std::endl;
  int pid = fork();
  if (pid < 0) {
    fprintf(stderr, "fork failed\n");
    exit(1);
  } else if (pid == 0) {
    // TODO: ./runner -> for macOS !!! but in csugrad idk...
    execvp(args[0], args);
    perror("child did not exit properly");
    _exit(1);
  } else {
    // extract args from array and store as std::string
    std::ostringstream ss;
    for (int i = 0; args[i] != NULL; ++i) {
      ss << args[i] << " ";
    }
    process_control_block.insert(std::pair<int, std::string>(pid, ss.str()));
  }
}

void RunSyncProcess(std::vector<std::string> &commands) {
  ioDetails ioArgs = GetIOArgs(commands);
  std::string in = "";
  std::string out = "";
  bool isInputFile = false;
  bool isOutputFile = false;
  int fdIN;
  int fdOUT;
  if (ioArgs.fileIN != "") {
    std::cout << "got file " << ioArgs.fileIN << std::endl;
    isInputFile = true;
    commands.erase(commands.begin() + ioArgs.indexIN);
    fdIN = open(ioArgs.fileIN.c_str(), O_RDONLY, S_IRUSR);
  }

  if (ioArgs.fileOUT != "") {
    std::cout << "output to file " << ioArgs.fileOUT << std::endl;
    isOutputFile = true;
    commands.erase(commands.begin() + ioArgs.indexOUT);
    fdOUT = open(ioArgs.fileOUT.c_str(), O_WRONLY, S_IRUSR | S_IWUSR);
  }
  char **args = GetBasicArgs(commands);
  int pid = fork();
  if (pid < 0) {
    fprintf(stderr, "fork failed\n");
    exit(1);
  } else if (pid == 0) {
    if (isInputFile) {
      std::cout << "duplicating file in" << std::endl;
      dup2(fdIN, STDIN_FILENO);
    }
    if (isOutputFile) {
      std::cout << "duplicating file out" << std::endl;
      dup2(fdOUT, STDOUT_FILENO);
    }
    execvp(args[0], args);
    perror("child did not exit properly");
    _exit(1);
  } else {
    std::ostringstream ss;
    for (int i = 0; args[i] != NULL; ++i) {
      ss << args[i] << " ";
    }
    process_control_block.insert(std::pair<int, std::string>(pid, ss.str()));
    int status;
    waitpid(pid, &status, 0);
    if (isInputFile) {
      std::cout << std::endl << std::flush;
      close(fdIN);
    }
    if (isOutputFile) {
      close(fdOUT);
    }
  }
}

void runNativeUserCommand(std::vector<std::string> const &commands) {
  switch (ArgToEnum(commands[0])) {
    case native_commands_enum::exit_command: {
      printf("<------ RUNNING EXIT ------>\n");
      ExitCommand();
      break;
    }
    case native_commands_enum::sleep_command: {
      printf("<------ RUNNING SLEEP ------>\n");
      SleepCommand(stoi(commands[1]));
      break;
    }
    case native_commands_enum::jobs_command: {
      printf("<------ RUNNING JOBS ------>\n");
      JobsCommand();
      break;
    }
    case native_commands_enum::kill_command: {
      printf("<------ RUNNING KILL ------>\n");
      KillCommand(stoi(commands[1]));
      break;
    }
    case native_commands_enum::resume_command: {
      printf("<------ RUNNING RESUME ------>\n");
      ResumeCommand(stoi(commands[1]));
      break;
    }
    case native_commands_enum::suspend_command: {
      printf("<------ RUNNING SUSPEND ------>\n");
      SuspendCommand(stoi(commands[1]));
      break;
    }
    case native_commands_enum::wait_command: {
      printf("<------ RUNNING WAIT ------>\n");
      WaitCommand(stoi(commands[1]));
      break;
    }
  }
}

bool isNativeCommand(std::string command) {
  return native_commands.find(command) != native_commands.end();
}

std::vector<std::string> getUserInput() {
  std::string input;
  std::cout << SHELL379 << std::flush;
  std::getline(std::cin, input);
  return SplitString(input);
}

bool IsBackgroundCommand(std::vector<std::string> command) {
  return command.at(command.size() - 1) == "&";
}

/**
 * Ctrl+C signal catch
 */
// TODO: stop current process in shell ????
void Interupt(int sig) {}

int main() {
  // Ctrl+C Signal catch
  signal(SIGINT, Interupt);
  int status;
  int background_pid;
  while (1) {
    std::vector<std::string> user_input = getUserInput();
    if (user_input.size() <= 0) {
      continue;
    }

    for (auto entry : process_control_block) {
      int pid = entry.first;
      background_pid = waitpid(pid, &status, WNOHANG);
      if (process_control_block.count(background_pid) == 1) {
        std::cout << "erasing PID: " << background_pid << " from PCB"
                  << std::endl;
        if (WIFEXITED(status)) {
          process_control_block.erase(background_pid);
        }
      }
    }

    if (isNativeCommand(user_input.at(0))) {
      runNativeUserCommand(user_input);
    } else {
      if (IsBackgroundCommand(user_input)) {
        user_input.pop_back();
        char **args = GetBasicArgs(user_input);
        RunBackgroundProcess(args);
      } else {
        RunSyncProcess(user_input);
      }
    }
  }
  return 0;
}