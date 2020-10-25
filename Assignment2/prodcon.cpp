#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include "cqueue.hpp"
#include "printer.hpp"
// instantiate global queue
ConcurrentQueue cq;
Printer p;
std::mutex printGuard;
void consumer() {
    int i = cq.Pop();
    p.Print(std::to_string(i));
}

void producer() {
}

int main(int argc, char *argv[]) {
    // take input from commands line

    int numThreads = 5;
    std::vector<std::thread> threadPool;
    for (int i = 0; i < numThreads; i++) {
        threadPool.emplace_back(std::thread(&consumer));
    }

    for (auto &t : threadPool) {
        if (t.joinable()) {
            t.join();
        }
    }

    return 0;
}