#include <iostream>
#include <string>
#include <set>
#include <regex>
#include <sstream>

#define LINE_LENGTH 100   // Max # of characters in an input line
#define MAX_ARGS 7        // Max number of arguments to a command
#define MAX_LENGTH 20     // Max # of characters in an argument
#define MAX_PT_ENTRIES 32 // Max entries in the Process Table

bool IsValidCommand(std::string command)
{
    std::set<std::string> valid_commands = {"exit", "jobs", "kill", "resume", "sleep", "suspend", "wait"};
    std::string valid_command_pattern = "([a-z]+)( ([a-z0-9><&]+))?";

    return false;
}

void GetCommands()
{
    std::string input;
    std::string primary_command;
    std::string secondary_command;

    std::cout << "SHELL379: ";
    std::getline(std::cin, input);
    if (IsValidCommand(input))
    {
    }

    std::istringstream ss(input);
    std::vector<std::string> command(std::istream_iterator<std::string>{ss}, std::istream_iterator<std::string>());
}

int main()
{

    while (1)
    {
        GetCommands();
    }

    return 0;
}