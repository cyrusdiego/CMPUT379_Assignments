#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <map>
#include <string>

#include "helpers.hpp"
#include "parser.hpp"

class Shell {
 public:
  Shell();
  ~Shell();
  void run();

 private:
  Parser parser;
  std::map<int, std::string> pcb;
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