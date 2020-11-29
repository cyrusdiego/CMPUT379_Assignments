#include "../include/logger.hpp"

Logger::Logger() {
}

void Logger::SetTitle(std::string title) {
    this->title = title;

    std::ofstream output;
    output.open(title, std::ofstream::out | std::ofstream::app);
    output.close();
}

void Logger::Log(double time, std::string message, std::string col) {
    std::ofstream output;
    std::stringstream ss;
    output.open(title, std::ofstream::out | std::ofstream::app);
    ss << std::fixed << std::setw(4) << std::left << col << " ";
    ss << std::fixed << std::setprecision(2) << time << ": " << message;
    output << ss.str() << std::flush;
    output.close();
}

void Logger::Log(std::string message, std::string col) {
    std::ofstream output;
    std::stringstream ss;
    output.open(title, std::ofstream::out | std::ofstream::app);
    ss << std::fixed << std::setw(4) << std::left << col << " ";
    ss << message;
    output << ss.str() << std::flush;
    output.close();
}

void Logger::Log(std::string message) {
    std::ofstream output;
    output.open(title, std::ofstream::out | std::ofstream::app);
    output << message << std::flush;
    output.close();
}

void Logger::PrintToScreen(std::string message) {
    std::cout << message << std::endl;
}

void Logger::PrintToScreen(double time, std::string message) {
    std::cout << std::fixed << std::setprecision(2) << time << ": " << message;
}