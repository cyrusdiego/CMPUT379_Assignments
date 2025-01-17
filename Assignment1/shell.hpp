#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <map>
#include <string>
#include <sys/resource.h>
#include "helpers.hpp"
#include "parser.hpp"

class Shell {
 public:
  Shell();
  ~Shell();
  void run();

 private:
  // PID: command, state
  std::map<int, std::pair<std::string, std::string>> pcb;
  Parser parser;
  std::vector<std::string> PCBTable;
  void UpdatePCB();
  void RunNativeCommand();
  void RunCommand();
  void SleepCommand(int seconds);
  void WaitCommand(int id);
  void SuspendCommand(int id);
  void ResumeCommand(int id);
  void KillCommand(int id);
  void ExitCommand();
  void JobsCommand();
  int OpenInputFile();
  int OpenOutputFile();
};
