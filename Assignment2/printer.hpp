#include <iostream>
#include <mutex>

class Printer {
   private:
    std::mutex printerMutex;

   public:
    Printer();
    ~Printer();
    void Print(std::string output);
};