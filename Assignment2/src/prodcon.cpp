#include <cctype>
#include <cstring>
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
std::mutex counter_mutex;
bool isEOF = false;

/**
 * Parse T<n> to a joby type and time
 */
std::pair<char, int> parse_job(std::string job) {
    char type = job.front();
    std::string s(job.begin() + 1, job.end());
    long int time = stoll(s);

    return std::pair<char, int>(type, time);
}

/**
 * Checks if a string is a positive 
 */
bool is_positive_number(char *arg) {
    int length = (int)std::strlen(arg);
    for (int i = 0; i < length; i++) {
        if (!std::isdigit(arg[i])) return false;
    }
    return true;
}

/**
 * Ensures:
 * - Valid number of input provided
 * - nthreads and id are positive integers
 */
std::pair<int, int> parse_input(int argc, char **argv) {
    if (argc > 3 || argc <= 1 || !is_positive_number(argv[1])) {
        perror(INVALID_ARGUMENTS.c_str());
        exit(-1);
    }
    int num_threads = atoi(argv[1]);

    int id = 0;
    if (argc == 3) {
        if (!is_positive_number(argv[2])) {
            perror(INVALID_ARGUMENTS.c_str());
            exit(-1);
        }
        id = atoi(argv[2]);
    }
    return std::pair<int, int>(num_threads, id);
}

/**
 * Thread-safe way to increment counters
 * TODO: guard entire stats struct then increment using if statements
 */
void incrementCounter(std::string job, int id = 0) {
    std::lock_guard<std::mutex> counter_guard(counter_mutex);

    if (job == ASK) {
        stats.ask++;
    } else if (job == RECEIVE) {
        stats.receive++;
    } else if (job == COMPLETE) {
        stats.complete++;
    } else if (job == WORK) {
        stats.work++;
    } else if (job == SLEEP) {
        stats.sleep++;
    } else {
        stats.work_count[id]++;
    }
}

/**
 * Consumer loop that will:
 * - Try to pop from queue
 * - if a valid job, runs Trans(n)
 */
void consumer(int id) {
    int my_id = id;

    while (!isEOF || cq->Size() > 0) {
        p->print(my_id, ASK);
        incrementCounter(ASK);
        int job = cq->Pop();
        if (job != -1) {
            p->print(my_id, RECEIVE, job, cq->Size());
            incrementCounter(RECEIVE);
            Trans(job);
            p->print(my_id, COMPLETE, job);
            incrementCounter(COMPLETE);
            incrementCounter("", my_id);
        }
    }
}

/**
 * Producer loop that will:
 * - read input from user or file
 * - if T<n> will push to queue
 * - if S<n> will sleep 
 */
void producer() {
    std::string input;
    while (std::getline(std::cin, input)) {
        auto job = parse_job(input);

        if (job.first == 'T') {
            incrementCounter(WORK);
            cq->Push(job.second);
            p->print(0, WORK, job.second, cq->Size());
        } else {
            p->print(0, SLEEP, job.second);
            Sleep(job.second);
            incrementCounter(SLEEP);
        }
    }
    isEOF = true;
    cq->NotifyConsumers(isEOF);
    p->print(0, END);
}

/**
 * Entry point for program
 * Initializes threads, global objects, and starts producer / consumer loops
 */
int main(int argc, char *argv[]) {
    // Checks user input
    auto args = parse_input(argc, argv);
    int num_consumers = args.first;
    int id = args.second;
    // // Initialize concurrent queue and thread safe printer
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

    // Delete threads
    for (auto &t : threadPool) {
        if (t.joinable()) {
            t.join();
        }
    }

    // prints final stats
    p->print(stats);

    // Free resources
    delete cq;
    delete p;

    return 0;
}