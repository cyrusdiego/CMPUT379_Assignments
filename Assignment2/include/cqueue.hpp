#include <condition_variable>
#include <mutex>
#include <queue>

class ConcurrentQueue {
   private:
    std::queue<int> tasks;
    std::mutex job_avail;
    std::mutex slot_avail;
    std::mutex queue_mutex;
    std::condition_variable is_slot_available;
    int max_size;

   public:
    ConcurrentQueue(int maxSize);
    ~ConcurrentQueue();
    void Push(int t);
    int Pop();
    int Size();
    std::condition_variable is_job_available;
};