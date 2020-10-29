#include "../include/prodcon.hpp"

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

int main(int argc, char *argv[]) {
    auto args = parse_input(argc, argv);
    int num_consumers = args.first;
    int id = args.second;

    ProdCon prodcon(id, num_consumers);
    // Initialize concurrent queue and thread safe printer
    // cq = new ConcurrentQueue(num_consumers * 2);
    // p = new Printer(id);

    // std::vector<std::thread> threadPool;

    // // All threads need some sort of ID
    // for (int i = 0; i < num_consumers; i++) {
    //     stats.work_count.insert({i + 1, 0});
    //     threadPool.emplace_back(std::thread(&consumer, i + 1));
    // }

    // // Main thread is producer
    // producer();

    // // Delete threads
    // for (auto &t : threadPool) {
    //     if (t.joinable()) {
    //         t.join();
    //     }
    // }

    // p->print(stats);

    // // Free resources
    // delete cq;
    // delete p;
    return 0;
}