#include <fstream>
#include <iostream>
#include <mutex>
#include <sstream>

#include "constants.hpp"

class Printer {
   private:
    std::mutex printer_mutex;
    std::string file_name;

   public:
    Printer(int id);
    ~Printer();
    void print(double time, int thread_id, std::string state, int n);
    void print(double time, int thread_id, std::string state, int n, int q_size);
};