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
    std::chrono::time_point<std::chrono::system_clock> start;

   public:
    Printer(int id);
    ~Printer();
    void print(int thread_id, std::string state, int n);
    void print(int thread_id, std::string state, int n, int q_size);
};

#endif