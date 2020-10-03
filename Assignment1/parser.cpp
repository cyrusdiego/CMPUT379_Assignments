#include "parser.hpp"

#include "helpers.hpp"
Parser::Parser() {}
Parser::~Parser() {}

void Parser::GetUserInput() {
  std::string input;
  std::vector<std::string> split_input;
  while (input == "") {
    std::cout << SHELL379 << std::flush;
    std::getline(std::cin, input);
    split_input = SplitStringToVector(input);
    input = this->ValidateInput(input, split_input);
  }

  this->commandsString = input;
  this->cmdVector = split_input;
}

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

std::string Parser::GetCommandsString() { return this->commandsString; }
std::vector<std::string> Parser::GetCommandsVector() { return this->cmdVector; }
char **Parser::GetArgs() { return this->args; }
bool Parser::GetIsBackgroundProcess() { return this->isBackgroundProcess; }
bool Parser::GetIsNativeCommand() { return this->isNativeCommand; }
bool Parser::GetIsInputRedirected() { return this->isInputRedirected; }
bool Parser::GetIsOutputRedirected() { return this->isOutputRedirected; }
std::string Parser::GetInputFile() { return this->ioFileDetails.fileIN; }
std::string Parser::GetOutputFile() { return this->ioFileDetails.fileOUT; }

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

void Parser::SetArgs() {
  this->args = ConvertVectorToCharArray(this->cmdVector);
}

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
