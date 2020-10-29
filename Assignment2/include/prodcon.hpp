#ifndef PRODCON_HPP
#define PRODCON_HPP
#include <string>
#include <thread>
#include <vector>

#include "../include/constants.hpp"
#include "../include/cqueue.hpp"
#include "../include/printer.hpp"
#include "../include/tands.hpp"

class ProdCon {
   private:
    void producer();
    void consumer(int id);
    std::pair<char, int> parse_job(std::string job);
    std::vector<std::thread> threadPool;
    ConcurrentQueue cq;
    Printer p;
    statistics stats;
    bool isEOF;

   public:
    ProdCon(int id, int size);
    ~ProdCon();
};

#endif