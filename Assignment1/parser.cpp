#include "parser.hpp"

#include "helpers.cpp"
Parser::Parser() {}
Parser::~Parser() {}

void Parser::GetUserInput() {
  std::string input = "";

  while (!input.empty()) {
    std::cout << SHELL379 << std::flush;
    std::getline(std::cin, input);
  }

  this->commandsString = input;
  this->cmdVector = helpers::SplitStringToVector(input);
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
  details.fileIN = fileIN;
  details.fileOUT = fileOUT;

  this->ioFileDetails = details;
}

void Parser::SetArgs() {
  this->args = helpers::ConvertVectorToCharArray(this->cmdVector);
}

void Parser::CleanUserInput() {
  std::vector<std::string> newCommands;
  for (int i = 0; i < this->cmdVector.size(); i++) {
    for (auto c : cmdVector.at(i)) {
      if (c != '<' || c != '>' || c != '&') {
        newCommands.push_back(cmdVector.at(i));
        break;
      }
    }
  }

  this->cmdVector = newCommands;
}
