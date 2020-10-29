#include "../include/printer.hpp"

Printer::Printer(int id) : start(std::chrono::system_clock::now()) {
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

void Printer::print(int thread_id, std::string state, int n) {
    std::lock_guard<std::mutex> printGuard(printer_mutex);
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> time = end - start;
    std::ofstream output;
    std::stringstream ss;

    output.open(file_name, std::ofstream::out | std::ofstream::app);
    ss << std::setprecision(3) << std::fixed
       << time.count()
       << ID_EQUALS << thread_id
       << std::setw(1)
       << state
       << std::setw(15 - state.size())
       << n
       << std::endl;

    output << ss.str() << std::flush;
    output.close();
}

void Printer::print(int thread_id, std::string state, int n, int q_size) {
    std::lock_guard<std::mutex> printGuard(printer_mutex);
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> time = end - start;
    std::ofstream output;
    std::stringstream ss;

    output.open(file_name, std::ofstream::out | std::ofstream::app);
    ss << std::setprecision(3) << std::fixed
       << time.count()
       << ID_EQUALS << thread_id
       << QUEUE_EQUALS << q_size
       << std::setw(1)
       << state
       << std::setw(15 - state.size())
       << n
       << std::endl;

    output << ss.str() << std::flush;
    output.close();
}