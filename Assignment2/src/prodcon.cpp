#include <string>
#include <thread>
#include <vector>

#include "../include/constants.hpp"
#include "../include/cqueue.hpp"
#include "../include/printer.hpp"
#include "../include/tands.hpp"

ConcurrentQueue *cq;
Printer *p;
statistics stats;
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

void consumer(int id) {
    int my_id = id;

    while (!isEOF || cq->Size() > 0) {
        p->print(my_id, ASK);
        stats.ask++;
        int job = cq->Pop();
        if (job != -1) {
            p->print(my_id, RECEIVE, job, cq->Size());
            stats.receive++;
            Trans(job);
            p->print(my_id, COMPLETE, job);
            stats.complete++;
            stats.work_count[my_id]++;
        }
    }
}

void producer() {
    std::string input;
    while (std::getline(std::cin, input)) {
        std::string state;
        auto job = parse_job(input);
        if (job.first == 'T') {
            p->print(0, WORK, job.second, cq->Size());
            stats.work++;
            cq->Push(job.second);
        } else {
            p->print(0, SLEEP, job.second);
            Sleep(job.second);
            stats.sleep++;
        }
    }
    isEOF = true;
    cq->NotifyConsumers(isEOF);
    p->print(0, END);
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
        stats.work_count.insert({i + 1, 0});
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

    p->print(stats);

    // Free resources
    delete cq;
    delete p;

    return 0;
}