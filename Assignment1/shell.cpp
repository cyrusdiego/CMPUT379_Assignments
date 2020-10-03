#include "shell.hpp"

Shell::Shell() : parser(), pcb() {}
Shell::~Shell() {}
void Shell::run() {
  while (1) {
    this->parser.GetUserInput();
    this->UpdatePCB();
    this->parser.ParseUserInput();

    bool isNativeCommand = this->parser.GetIsNativeCommand();
    if (isNativeCommand) {
      this->RunNativeCommand();
    } else {
      this->RunCommand();
    }
  }
}

void Shell::UpdatePCB() {
  int pid;
  int background_pid;
  int status;
  for (auto entry : this->pcb) {
    pid = entry.first;
    background_pid = waitpid(pid, &status, WNOHANG);
    if (WIFEXITED(status)) {
      this->pcb.erase(background_pid);
    }
  }
}

void Shell::RunNativeCommand() {
  auto cmd = this->parser.GetCommandsVector();
  if (!this->parser.HasValidArg(cmd)) {
    fprintf(stderr, "%s", INVALID_ARGUMENTS.c_str());
    return;
  }

  switch (NativeCommandToEnum(cmd.at(0))) {
    case native_commands_enum::exit_command: {
      ExitCommand();
      break;
    }
    case native_commands_enum::sleep_command: {
      SleepCommand(stoi(cmd.at(1)));
      break;
    }
    case native_commands_enum::jobs_command: {
      JobsCommand();
      break;
    }
    case native_commands_enum::kill_command: {
      KillCommand(stoi(cmd.at(1)));
      break;
    }
    case native_commands_enum::resume_command: {
      ResumeCommand(stoi(cmd.at(1)));
      break;
    }
    case native_commands_enum::suspend_command: {
      SuspendCommand(stoi(cmd.at(1)));
      break;
    }
    case native_commands_enum::wait_command: {
      WaitCommand(stoi(cmd.at(1)));
      break;
    }
  }
}

// WORKS Background, file I/O, sync
void Shell::RunCommand() {
  bool isBackgroundProcess = this->parser.GetIsBackgroundProcess();
  if (isBackgroundProcess && this->pcb.size() >= MAX_PT_ENTRIES) {
    fprintf(stderr, "%s", TOO_MANY_BACKGROUND_PROCESSES.c_str());
    return;
  }
  char **args = this->parser.GetArgs();
  bool isInputRedirected = this->parser.GetIsInputRedirected();
  bool isOutputRedirected = this->parser.GetIsOutputRedirected();
  int inputFileID, outputFileID;
  if (isInputRedirected) inputFileID = OpenInputFile();
  if (isOutputRedirected) outputFileID = OpenOutputFile();

  int pid = fork();
  if (pid < 0) {
    fprintf(stderr, "%s", FORK_FAILED.c_str());
    exit(1);
  } else if (pid == 0) {
    if (isInputRedirected) {
      dup2(inputFileID, STDIN_FILENO);
    }
    if (isOutputRedirected) {
      dup2(outputFileID, STDOUT_FILENO);
    }
    execvp(args[0], args);
    perror(EXEC_FAILED.c_str());
    _exit(1);
  } else {
    int status;
    int waitFlag = 0;
    if (isBackgroundProcess) {
      std::string command = this->parser.GetCommandsString();
      std::pair<std::string, std::string> entry(command, "R+");
      this->pcb.insert(
          std::pair<int, std::pair<std::string, std::string>>(pid, entry));
      pid = -1;
      waitFlag = WNOHANG;
    }

    waitpid(pid, &status, waitFlag);

    if (isInputRedirected) {
      close(inputFileID);
    }
    if (isOutputRedirected) {
      close(outputFileID);
    }
  }
}

// WORKS
int Shell::OpenInputFile() {
  auto file_name = this->parser.GetInputFile();
  int fid = open(file_name.c_str(), O_RDONLY, S_IRUSR | S_IWUSR);
  if (fid < 0) {
    perror(OPEN_FAILED.c_str());
    _exit(1);
  }
  return fid;
}

// WORKS
int Shell::OpenOutputFile() {
  auto file_name = this->parser.GetOutputFile();
  int fid = open(file_name.c_str(), O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);
  if (fid < 0) {
    perror(OPEN_FAILED.c_str());
    _exit(1);
  }
  return fid;
}

// WORKS
void Shell::JobsCommand() {
  std::vector<std::string> ps_results;
  std::string PCBTable;
  std::ostringstream ss;
  int active_processes = 0;
  int shell_379_processes = 0;
  time_t user_time;
  time_t sys_time;
  struct rusage usage;
  FILE *p;

  if ((p = popen(PS_COMMAND.c_str(), "r")) == NULL) {
    printf("%s", PS_ERROR.c_str());
  } else {
    // Parse through ps results
    ps_results = this->parser.ReadPSResults(p);
    // Get Usage Times
    getrusage(RUSAGE_CHILDREN, &usage);
    user_time = usage.ru_utime.tv_sec;
    sys_time = usage.ru_stime.tv_sec;

    // // Build PCB table from ps results
    PCBTable =
        this->parser.BuildJobsTable(this->pcb, ps_results, user_time, sys_time);

    // // Print Jobs Table
    std::cout << PCBTable;
    std::cout << std::endl << std::flush;
  }
}

// WORKS
void Shell::ExitCommand() {
  for (auto entry : this->pcb) {
    std::pair<int, std::pair<std::string, std::string>> process = entry;
    int pid = process.first;
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

// WORKS
void Shell::KillCommand(int id) {
  if (kill(id, SIGKILL) < 0) {
    perror(KILL_FAILED.c_str());
    _exit(1);
  }
  int status;
  waitpid(id, &status, 0);
}

// WORKS
void Shell::ResumeCommand(int id) {
  if (kill(id, SIGCONT) < 0) {
    perror(KILL_FAILED.c_str());
    _exit(1);
  }
  int status;
  this->pcb.at(id).second = "R+";

  waitpid(id, &status, WNOHANG);
}

// WORKS
void Shell::SuspendCommand(int id) {
  if (kill(id, SIGSTOP) < 0) {
    perror(KILL_FAILED.c_str());
    _exit(1);
  }
  int status;
  this->pcb.at(id).second = "T+";

  waitpid(id, &status, WNOHANG);
}

// WORKS
void Shell::WaitCommand(int id) {
  int status;
  waitpid(id, &status, 0);
}

// WORKS
void Shell::SleepCommand(int seconds) { sleep(seconds); }
