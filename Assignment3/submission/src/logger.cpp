#include "../include/logger.hpp"

Logger::Logger() {
}

/**
 * Creates log file and sets the title for it
 */
void Logger::SetTitle(std::string title) {
    this->title = title;

    std::ofstream output;
    output.open(title, std::ofstream::out | std::ofstream::app);
    output.close();
}

/**
 * Logs to the log file
 */
void Logger::Log(double time, std::string message, std::string col) {
    std::ofstream output;
    std::stringstream ss;
    output.open(title, std::ofstream::out | std::ofstream::app);
    ss << std::fixed << std::setw(4) << std::left << col << " ";
    ss << std::fixed << std::setprecision(2) << time << ": " << message;
    output << ss.str() << std::flush;
    output.close();
}

/**
 * Logs to the log file
 */
void Logger::Log(std::string message, std::string col) {
    std::ofstream output;
    std::stringstream ss;
    output.open(title, std::ofstream::out | std::ofstream::app);
    ss << std::fixed << std::setw(4) << std::left << col << " ";
    ss << message;
    output << ss.str() << std::flush;
    output.close();
}

/**
 * Logs to the log file
 */
void Logger::Log(std::string message) {
    std::ofstream output;
    output.open(title, std::ofstream::out | std::ofstream::app);
    output << message << std::flush;
    output.close();
}