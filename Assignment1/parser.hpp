#include <algorithm>
#include <fstream>
#include <iostream>
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
  std::string GetCommandsString();
  std::vector<std::string> GetCommandsVector();
  char** GetArgs();
  bool GetIsBackgroundProcess();
  bool GetIsNativeCommand();
  bool GetIsInputRedirected();
  bool GetIsOutputRedirected();
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
};