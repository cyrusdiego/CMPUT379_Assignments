#ifndef CONSTANTS_H
#define CONSTANTS_H
#include <string>
#include <unordered_map>

const std::string ASK = "Ask";
const std::string RECEIVE = "Receive";
const std::string WORK = "Work";
const std::string SLEEP = "Sleep";
const std::string COMPLETE = "Complete";
const std::string END = "End";
const std::string QUEUE_EQUALS = "Q=";
const std::string ID_EQUALS = "ID=";
const std::string INVALID_ARGUMENTS = "Invalid arguments: requires: nthreads <id>. Exiting program.";
const std::string SUMMARY = "Summary:";
const std::string THREAD = "Thread";
const std::string TRANS_PER_SEC = "Transactions per second: ";

struct statistics {
    int work = 0;
    int ask = 0;
    int receive = 0;
    int complete = 0;
    int sleep = 0;
    std::unordered_map<int, int> work_count;
};

#endif