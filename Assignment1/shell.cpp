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
  // TODO: DO ERROR CHECKING FOR THE INDEXING HERE JFC
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

void Shell::RunCommand() {
  bool isBackgroundProcess = this->parser.GetIsBackgroundProcess();
  if (isBackgroundProcess && this->pcb.size() >= MAX_PT_ENTRIES) {
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
    fprintf(stderr, "fork failed\n");
    exit(1);
  } else if (pid == 0) {
    if (isInputRedirected) {
      dup2(inputFileID, STDIN_FILENO);
    }
    if (isOutputRedirected) {
      dup2(outputFileID, STDOUT_FILENO);
    }
    execvp(args[0], args);
    perror("child did not exit properly");
    _exit(1);
  } else {
    int status;
    int waitFlag = 0;
    if (isBackgroundProcess) {
      std::string command = this->parser.GetCommandsString();
      this->pcb.insert(std::pair<int, std::string>(pid, command));
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

int Shell::OpenInputFile() {
  auto file_name = this->parser.GetInputFile();
  int fid = open(file_name.c_str(), O_RDONLY, S_IRUSR | S_IWUSR);
  if (fid < 0) {
    perror("open()");
    _exit(1);
  }
  return fid;
}

int Shell::OpenOutputFile() {
  auto file_name = this->parser.GetOutputFile();
  int fid = open(file_name.c_str(), O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);
  if (fid < 0) {
    perror("open()");
    _exit(1);
  }
  return fid;
}

void Shell::JobsCommand() {
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
    ps_table = SplitStringToVector(ps_results);
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
      if (this->pcb.count(pid) == 1) {
        // look for active processes
        if (state == "R") {  // TODO: need to just check if R is in there
          active_processes++;
        }

        // build PCB row entry
        ss << shell_379_processes << ": " << pid << " " << state << " " << time
           << " " << this->pcb.at(pid);
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
    for (auto x : this->pcb) {
      std::cout << "PCB entry: " << x.first << " " << x.second;
    }
    std::cout << std::endl << std::flush;
  }
}

void Shell::ExitCommand() {
  std::cout << "size of PCB: " << this->pcb.size() << std::endl;
  for (auto entry : this->pcb) {
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

void Shell::KillCommand(int id) {
  if (kill(id, SIGKILL) < 0) {
    perror("kill failed");
    _exit(1);
  }
  int status;
  waitpid(id, &status, 0);
}

void Shell::ResumeCommand(int id) {
  if (kill(id, SIGCONT) < 0) {
    perror("kill failed");
    _exit(1);
  }
  int status;
  waitpid(id, &status, 0);
}

void Shell::SuspendCommand(int id) {
  if (kill(id, SIGSTOP) < 0) {
    perror("kill failed");
    _exit(1);
  }
  int status;
  waitpid(id, &status, 0);
}

void Shell::WaitCommand(int id) {
  int status;
  waitpid(id, &status, 0);
}

void Shell::SleepCommand(int seconds) { sleep(seconds); }
