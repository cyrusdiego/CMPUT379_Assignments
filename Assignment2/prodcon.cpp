#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include "cqueue.hpp"
#include "tands.hpp"

// instantiate global queue
ConcurrentQueue *cq;
std::mutex printMutex;

void print(std::string output) {
    std::lock_guard<std::mutex> printGuard(printMutex);
    std::cout << output << std::endl;
}

std::pair<char, int> parseJob(std::string job) {
    char type = job.front();
    std::string s(job.begin() + 1, job.end());
    long int time = stoll(s);

    return std::pair<char, int>(type, time);
}

void consumer() {
    while (1) {
        std::string job = cq->Pop();
        std::pair<char, int> task = parseJob(job);
        if (task.first == 'T') {
            Trans(task.second);
        } else {
            Sleep(task.second);
        }
        print(job);
    }
}

void producer() {
    std::vector<std::string> tasks = {"T50", "T20", "T455", "S10000", "T3", "S12000"};
    for (auto i : tasks) {
        cq->Push(i);
    }

    print("end of for loop");
}

int main(int argc, char *argv[]) {
    // take input from commands line

    int numConsumers = 2;
    cq = new ConcurrentQueue(numConsumers * 2);
    std::vector<std::thread> threadPool;

    for (int i = 0; i < numConsumers; i++) {
        threadPool.emplace_back(std::thread(&consumer));
    }

    producer();

    // Delete threads
    for (auto &t : threadPool) {
        if (t.joinable()) {
            t.join();
        }
    }

    // Free resources
    delete cq;

    return 0;
}