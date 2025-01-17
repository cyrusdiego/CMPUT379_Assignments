#include "../include/printer.hpp"

/**
 * starts timer and creates file
 */
Printer::Printer(int id) : start(std::chrono::system_clock::now()) {
    std::ofstream output;

    if (id == 0) {
        file_name = "prodcon.log";
        output.open(file_name, std::ofstream::out | std::ofstream::app);
        output.close();
    } else {
        std::stringstream ss;
        ss << "prodcon." << id << ".log";
        file_name = ss.str();
        output.open(file_name, std::ofstream::out | std::ofstream::app);
        output.close();
    }
}

Printer::~Printer() {}

/**
 * prints row in log file using thread id, state, n, and queue size
 */
void Printer::print(int thread_id, std::string state, int n, int q_size) {
    std::lock_guard<std::mutex> printGuard(printer_mutex);
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> time = end - start;
    std::ofstream output;
    std::stringstream ss;

    output.open(file_name, std::ofstream::out | std::ofstream::app);
    ss << std::setprecision(3)
       << std::fixed
       << std::setw(7)
       << std::left
       << time.count();

    ss << std::setw(4) << std::left << ID_EQUALS << std::setw(3) << thread_id;

    if (q_size != -1) {
        ss << std::setw(3) << std::left << QUEUE_EQUALS << std::setw(3) << q_size;
    } else {
        ss << std::setw(6) << "";
    }

    ss << std::setw(10) << std::left << state;

    if (n != -1) {
        ss << std::setw(2) << n;
    }

    ss << std::endl;

    output << ss.str() << std::flush;
    output.close();
}

/**
 * prints end of program stats
 */
void Printer::print(statistics stats) {
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> time = end - start;
    std::ofstream output;
    std::stringstream ss;

    output.open(file_name, std::ofstream::out | std::ofstream::app);
    ss << SUMMARY << std::endl;
    ss << std::setw(4) << "" << std::setw(10) << std::left << WORK;
    ss << std::setw(2) << std::right << stats.work << std::endl;
    ss << std::setw(4) << "" << std::setw(10) << std::left << ASK;
    ss << std::setw(2) << std::right << stats.ask << std::endl;
    ss << std::setw(4) << "" << std::setw(10) << std::left << RECEIVE;
    ss << std::setw(2) << std::right << stats.receive << std::endl;
    ss << std::setw(4) << "" << std::setw(10) << std::left << COMPLETE;
    ss << std::setw(2) << std::right << stats.complete << std::endl;
    ss << std::setw(4) << "" << std::setw(10) << std::left << SLEEP;
    ss << std::setw(2) << std::right << stats.sleep << std::endl;

    for (auto w : stats.work_count) {
        ss << std::setw(4) << "" << std::setw(7) << std::left << THREAD;
        ss << std::setw(3) << std::left << w.first;
        ss << std::setw(2) << std::right << w.second << std::endl;
    }
    double transactions_per_sec = stats.work / time.count();
    ss << std::setprecision(4) << TRANS_PER_SEC << transactions_per_sec;
    output << ss.str() << std::flush;
    output.close();
}