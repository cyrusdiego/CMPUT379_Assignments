#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "constants.hpp"

struct ioDetails {
  std::string fileIN;
  std::string fileOUT;
};

class Parser {
 public:
  Parser();
  ~Parser();

  void GetUserInput();
  void ParseUserInput();
  std::vector<std::string> ReadPSResults(FILE* p);
  std::string BuildJobsTable(std::map<int, std::string> const& pcb,
                             std::vector<std::string> ps_results,
                             time_t user_time, time_t sys_time);
  std::string GetCommandsString();
  std::vector<std::string> GetCommandsVector();
  char** GetArgs();
  bool GetIsBackgroundProcess();
  bool GetIsNativeCommand();
  bool GetIsInputRedirected();
  bool GetIsOutputRedirected();
  bool HasValidArg(std::vector<std::string> cmd);
  std::string GetInputFile();
  std::string GetOutputFile();

 private:
  ioDetails ioFileDetails;
  std::vector<std::string> cmdVector;
  std::string commandsString;
  char** args;
  bool isBackgroundProcess;
  bool isNativeCommand;
  bool isInputRedirected;
  bool isOutputRedirected;

  void ParseForIO();
  void SetArgs();
  void CleanUserInput();
  std::string ValidateInput(std::string input,
                            std::vector<std::string> split_input);
  bool IsStateRunning(std::string state);
};