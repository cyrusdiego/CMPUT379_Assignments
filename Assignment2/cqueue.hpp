#include <mutex>
#include <queue>

class ConcurrentQueue {
   private:
    std::queue<int> tasks;
    std::mutex tasksMutex;

   public:
    ConcurrentQueue();
    ~ConcurrentQueue();
    void Push(int t);
    int Pop();
    int Size();
};