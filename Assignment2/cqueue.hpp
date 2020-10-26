#include <condition_variable>
#include <mutex>
#include <queue>

class ConcurrentQueue {
   private:
    std::queue<std::string> tasks;
    std::mutex can_push;
    std::mutex can_pop;
    std::mutex queue_mutex;
    std::condition_variable is_job_available;
    std::condition_variable is_slot_available;
    int max_size;

   public:
    ConcurrentQueue(int maxSize);
    ~ConcurrentQueue();
    void Push(std::string t);
    std::string Pop();
    int Size();
};