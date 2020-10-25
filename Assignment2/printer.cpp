#include "printer.hpp"

Printer::Printer() {}
Printer::~Printer() {}

void Printer::Print(std::string output) {
    std::lock_guard<std::mutex> printGuard(this->printerMutex);
    std::cout << output << std::endl;
}