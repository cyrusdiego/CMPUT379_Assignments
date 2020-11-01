#ifndef CQUEUE_HPP
#define CQUEUE_HPP
#include <condition_variable>
#include <mutex>
#include <queue>

class ConcurrentQueue {
   private:
    std::queue<int> tasks;
    std::mutex job_avail;
    std::mutex slot_avail;
    std::mutex queue_mutex;
    std::mutex size_mutex;
    std::condition_variable is_slot_available;
    std::condition_variable is_job_available;

    unsigned int max_size;
    bool isEOF;

   public:
    ConcurrentQueue(int maxSize);
    ~ConcurrentQueue();
    void Push(int t);
    int Pop();
    int Size();
    void NotifyConsumers(bool done);
};

#endif