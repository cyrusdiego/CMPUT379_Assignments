#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include "../include/constants.hpp"
#include "../include/cqueue.hpp"
#include "../include/printer.hpp"
#include "../include/tands.hpp"

ConcurrentQueue *cq;
Printer *p;
bool isEOF = false;

std::pair<char, int> parse_job(std::string job) {
    char type = job.front();
    std::string s(job.begin() + 1, job.end());
    long int time = stoll(s);

    return std::pair<char, int>(type, time);
}

std::pair<int, int> parse_input(int argc, char **argv) {
    if (argc > 3 || argc <= 1) {
        std::cout << INVALID_ARGUMENTS << std::endl;
        exit(-1);
    }

    int num_threads = atoi(argv[1]);

    int id = 0;
    if (argc == 3) {
        id = atoi(argv[2]);
    }

    return std::pair<int, int>(num_threads, id);
}

// states: Ask, Receive, Work, Complete
void consumer(int id) {
    int my_id = id;

    // will need to figure out how to kill or stop this loop when
    // all work is done!!
    while (!isEOF || cq->Size() > 0) {
        int job = cq->Pop();
        if (job != -1) {
            Trans(job);
            p->print(my_id, WORK, job);
        }
    }
}

// states: Receive, Sleep, End
void producer() {
    std::string input;
    while (std::getline(std::cin, input)) {
        std::string state;
        auto job = parse_job(input);
        if (job.first == 'T') {
            state = WORK;
            cq->Push(job.second);
        } else {
            state = SLEEP;
            Sleep(job.second);
            p->print(0, state, job.second);
        }
    }
    isEOF = true;
    cq->NotifyConsumers(isEOF);
}

int main(int argc, char *argv[]) {
    auto args = parse_input(argc, argv);
    int num_consumers = args.first;
    int id = args.second;

    // Initialize concurrent queue and thread safe printer
    cq = new ConcurrentQueue(num_consumers * 2);
    p = new Printer(id);

    std::vector<std::thread> threadPool;

    // All threads need some sort of ID
    for (int i = 0; i < num_consumers; i++) {
        threadPool.emplace_back(std::thread(&consumer, i + 1));
    }

    // Main thread is producer
    producer();

    // when main thread exits producer(), will need to wait for
    // consumers to finish their work

    // Delete threads
    for (auto &t : threadPool) {
        if (t.joinable()) {
            t.join();
        }
    }

    // Free resources
    delete cq;
    delete p;

    return 0;
}