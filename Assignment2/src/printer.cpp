#include "../include/printer.hpp"

Printer::Printer(int id) {
    if (id == 0) {
        // TODO: need to include an output directory in submission .zip!!
        file_name = "../output/prodcon.log";
    } else {
        std::stringstream ss;
        ss << "../output/prodcon." << id << ".log";
        file_name = ss.str();
    }
}

Printer::~Printer() {}

void Printer::print(double time, int thread_id, std::string state, int n) {
    std::lock_guard<std::mutex> printGuard(printer_mutex);
    std::ofstream output;
    std::stringstream ss;

    output.open(file_name, std::ofstream::out | std::ofstream::app);
    ss << std::setprecision(3) << std::fixed
       << time
       << ID_EQUALS << thread_id
       << std::setw(QUEUE_EQUALS.size() + 1)
       << state
       << std::setw(15 - state.size())
       << n
       << std::endl;

    output << ss.str();
    output.close();
}

void Printer::print(double time, int thread_id, std::string state, int n, int q_size) {
    std::lock_guard<std::mutex> printGuard(printer_mutex);
    std::ofstream output;
    std::stringstream ss;

    output.open(file_name, std::ofstream::out | std::ofstream::app);
    ss << std::setprecision(3) << std::fixed
       << time
       << ID_EQUALS << thread_id
       << QUEUE_EQUALS << q_size
       << std::setw(1)
       << state
       << std::setw(15 - state.size())
       << n
       << std::endl;

    output << ss.str();
    output.close();
}