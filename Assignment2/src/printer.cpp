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

void Printer::print(int thread_id, std::string state, int n, int q_size) {
    std::lock_guard<std::mutex> printGuard(printer_mutex);
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> time = end - start;
    std::ofstream output;
    std::stringstream ss;

    output.open(file_name, std::ofstream::out | std::ofstream::app);
    ss << std::setprecision(3)
       << std::fixed
       << std::setw(6)
       << std::left
       << time.count();

    ss << std::setw(4) << std::left << ID_EQUALS << std::setw(2) << thread_id;

    if (q_size != -1) {
        ss << std::setw(3) << std::left << QUEUE_EQUALS << std::setw(2) << q_size;
    } else {
        ss << std::setw(5) << "";
    }

    ss << std::setw(10) << std::left << state;

    if (n != -1) {
        ss << std::setw(2) << n;
    }

    ss << std::endl;

    output << ss.str() << std::flush;
    output.close();
}