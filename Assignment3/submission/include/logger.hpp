#ifndef LOGGER_HPP
#define LOGGER_HPP
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

class Logger {
   public:
    Logger();
    void Log(double time, std::string message, std::string col = "");
    void Log(std::string message, std::string col);
    void Log(std::string message);
    void SetTitle(std::string title);

   private:
    std::string title;
};

#endif