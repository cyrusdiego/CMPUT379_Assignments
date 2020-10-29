#ifndef PRINTER_HPP
#define PRINTER_HPP
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>

#include "constants.hpp"

class Printer {
   private:
    std::mutex printer_mutex;
    std::string file_name;
    std::chrono::time_point<std::chrono::high_resolution_clock> start;

   public:
    Printer(int id);
    ~Printer();
    void print(int thread_id, std::string state, int n = -1, int q_size = -1);
    void print(statistics stats);
};

#endif