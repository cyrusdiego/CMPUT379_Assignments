#include "parser.hpp"

#include "helpers.hpp"

Parser::Parser() { this->args = new char *[MAX_ARGS + 1]; }
Parser::~Parser() { delete[] this->args; }

/**
 * Step 1 of shell: get user input
 */
void Parser::GetUserInput() {
  std::string input;
  std::vector<std::string> split_input;
  while (input == "") {
    std::cout << SHELL379 << std::flush;
    std::getline(std::cin, input);
    split_input = SplitStringToVector(input, ' ');
    input = this->ValidateInput(input, split_input);
  }

  this->commandsString = input;
  this->cmdVector = split_input;
}

/**
 * Step 1 of shell: parse through input
 *
 * sets certain flags based on input
 */
void Parser::ParseUserInput() {
  this->isNativeCommand =
      native_commands.find(this->cmdVector.at(0)) != native_commands.end();
  this->isBackgroundProcess =
      this->cmdVector.at(this->cmdVector.size() - 1) == "&";

  this->ParseForIO();
  this->isInputRedirected = (this->ioFileDetails.fileIN == "") ? false : true;
  this->isOutputRedirected = (this->ioFileDetails.fileOUT == "") ? false : true;

  this->CleanUserInput();
  this->SetArgs();
}

/**
 * Parser class's getter functions
 */
std::string Parser::GetCommandsString() { return this->commandsString; }
std::vector<std::string> Parser::GetCommandsVector() { return this->cmdVector; }
char **Parser::GetArgs() { return this->args; }
bool Parser::GetIsBackgroundProcess() { return this->isBackgroundProcess; }
bool Parser::GetIsNativeCommand() { return this->isNativeCommand; }
bool Parser::GetIsInputRedirected() { return this->isInputRedirected; }
bool Parser::GetIsOutputRedirected() { return this->isOutputRedirected; }
std::string Parser::GetInputFile() { return this->ioFileDetails.fileIN; }
std::string Parser::GetOutputFile() { return this->ioFileDetails.fileOUT; }

/**
 * Validate accompanied argument for native commands
 */
bool Parser::HasValidArg(std::vector<std::string> cmd) {
  switch (NativeCommandToEnum(cmd.at(0))) {
    case native_commands_enum::exit_command:
    case native_commands_enum::jobs_command: {
      return cmd.size() == 1;
    }
    case native_commands_enum::sleep_command:
    case native_commands_enum::kill_command:
    case native_commands_enum::resume_command:
    case native_commands_enum::suspend_command:
    case native_commands_enum::wait_command: {
      return cmd.size() == 2 && IsInteger(cmd.at(1));
    }
  }
}

/**
 * Check if user input has file redirection
 */
void Parser::ParseForIO() {
  std::string fileIN = "";
  std::string fileOUT = "";
  int counter = 0;
  for (auto cmd : this->cmdVector) {
    for (auto letter : cmd) {
      if (letter == '<') {
        fileIN = cmd;
        break;
      }

      if (letter == '>') {
        fileOUT = cmd;
        break;
      }
    }
    counter++;
  }
  ioDetails details;
  details.fileIN = fileIN.erase(0, 1);
  details.fileOUT = fileOUT.erase(0, 1);

  this->ioFileDetails = details;
}

/**
 * converts string vector to char**
 */
void Parser::ConvertVectorToCharArray(std::vector<std::string> const &commandList) {
  int size = commandList.size();
  int index = 0;

  for (auto cmd : commandList) {
    char *temp = strdup(cmd.c_str());
    this->args[index] = strdup(cmd.c_str());
    free(temp);
    index++;
  }

  // Needs to be NULL terminated
  this->args[size] = NULL;
}

/**
 * Stores user input as char**
 */
void Parser::SetArgs() {
  ConvertVectorToCharArray(this->cmdVector);
}

/**
 * Removes <,>,& from user input
 */
void Parser::CleanUserInput() {
  std::vector<std::string> newCommands;
  for (auto cmd : this->cmdVector) {
    char c = cmd.at(0);
    if (c != '<' && c != '>' && c != '&') {
      newCommands.push_back(cmd);
    }
  }

  this->cmdVector = newCommands;
}

/**
 * Validates user input based on spec
 */
std::string Parser::ValidateInput(std::string input,
                                  std::vector<std::string> split_input) {
  int line_length = input.size();
  int args_count = split_input.size();
  int max_arg_length = -1;

  for (auto arg : split_input) {
    int arg_size = arg.size();
    max_arg_length = std::max(arg_size, max_arg_length);
  }

  if (line_length >= LINE_LENGTH || args_count >= MAX_ARGS ||
      max_arg_length >= MAX_LENGTH) {
    return "";
  }

  return input;
}

/**
 * Call ps command and read character by character
 */
std::vector<std::string> Parser::ReadPSResults(FILE *p) {
  int c;
  std::string psResults;
  while ((c = fgetc(p)) != EOF) {
    psResults.push_back(c);
  }
  pclose(p);
  return SplitStringToVector(psResults, ' ');
}

/**
 * Pass in time from ps command and convert to seconds
 */
std::string Parser::FormatTime(std::string rawTime) {
  std::vector<std::string> splitTime = SplitStringToVector(rawTime, ':');
  int multiplier = 1;
  int seconds = 0;

  for (int i = splitTime.size() - 1; i > 0; i--) {
    seconds += std::stof(splitTime.at(i).c_str());
    multiplier *= 60;
  }
  return std::to_string(seconds);
}

/**
 * Checks if current state is in running state
 */
bool Parser::IsStateRunning(std::string state) { return state.at(0) == 'R'; }

/**
 * Builds table for Jobs Command
 */
std::string Parser::BuildJobsTable(
    std::map<int, std::pair<std::string, std::string>> &pcb,
    std::vector<std::string> ps_results, time_t user_time, time_t sys_time) {
  std::ostringstream ss;
  std::string jobs_row;
  std::vector<std::string> jobs_table;
  int active_processes = 0;
  int shell_379_processes = 0;

  // skip first 3 entries as they're just the columns
  // and go in sets of 3 to get contents of row
  for (int i = 3; i < ps_results.size(); i += 3) {
    // get each column entry: pid, state, time
    int pid = stoi(ps_results[i]);
    std::string state = ps_results[i + 1];
    std::string time = ps_results[i + 2];
    time = this->FormatTime(time);
    // only look at processes started by shell379
    if (pcb.count(pid) == 1) {
      pcb.at(pid).second = state;
      // look for active processes
      if (this->IsStateRunning(state)) {
        active_processes++;
      }

      // build PCB row entry
      ss << shell_379_processes << ": " << pid << " " << state << " " << time
         << " " << pcb.at(pid).first;
      jobs_row = ss.str();

      // clear stream so i can read next row in ps results
      ss.str("");
      ss.clear();

      // store row into table
      jobs_table.push_back(jobs_row);
      shell_379_processes++;
    }
  }

  // // Build string to print Jobs Table
  ss << RUNNING_PROCESS << PCB_COLUMNS;
  for (int i = 0; i < jobs_table.size(); i++) {
    ss << jobs_table[i] << "\n";
  }

  // // Add user and sys times to Jobs Table
  ss << PROCESSES << active_processes << ACTIVE << COMPLETED_PROCESSES
     << USER_TIME << user_time << SECONDS << SYS_TIME << sys_time << SECONDS;

  return ss.str();
}
