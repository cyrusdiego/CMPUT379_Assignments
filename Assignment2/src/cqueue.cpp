#include "../include/cqueue.hpp"

/**
 * Choice of synchronization method: std::condition variables, std::mutex, std::lock_guard
 * 
 * C++ provides easy to use tools for multi-threading synchronization. Originally, I was going 
 * to use C's semaphores (semaphore.h i.e. sem_t) however I wanted to utilize C++'s standard library 
 * (semaphores are in C++20 not 11).
 * 
 * Reasons to use STL for synchronization:
 * 
 * 1) std::lock_guard unlocks mutexes when they go out of scope so it provides easy management 
 * 
 * 2) std::condition_variable 's wait() function provides syntactic sugar to do readible and clean predicate
 * checks:
 * For example, 
 *      std::condition_variable my_condition_var;
 *      std::unique_lock<std::mutex> my_guard(my_mutex);
 *      my_condition_var.wait(my_guard, [this] { return predicate(); });
 * 
 *  Translates to:
 *      std::condition_variable my_condition_var;
 *      std::unique_lock<std::mutex> my_guard(my_mutex);
 *      while(!predicate()) {
 *           my_condition_var.wait(my_guard);
 *      }
 *      
 * https://en.cppreference.com/w/cpp/thread/condition_variable highlights the usage for condition variables
 * 
 * 3) std::unique_lock will auto-release inside the condition variable wait predicate
 * 
 * Resources:
 * https://en.cppreference.com/w/cpp/thread/condition_variable 
 * https://en.cppreference.com/w/cpp/thread/unique_lock
 * https://en.cppreference.com/w/cpp/thread/mutex
 * https://en.cppreference.com/w/cpp/thread/lock_guard
 */

/**
 * constructs queue, max size, and EOF flag
 */
ConcurrentQueue::ConcurrentQueue(int size) : tasks(std::queue<int>()), max_size(size), isEOF(false) {}

ConcurrentQueue::~ConcurrentQueue() {}

/**
 * thread safe push into the queue 
 */
void ConcurrentQueue::Push(int t) {
    std::unique_lock<std::mutex> slot_avail_guard(slot_avail);
    is_slot_available.wait(slot_avail_guard, [this] { return tasks.size() < max_size; });
    std::lock_guard<std::mutex> push_guard(job_avail);
    std::lock_guard<std::mutex> queue_guard(queue_mutex);
    tasks.push(t);
    is_job_available.notify_one();
}

/**
 * thread safe pop into queue
 */
int ConcurrentQueue::Pop() {
    std::unique_lock<std::mutex> job_avail_guard(job_avail);
    is_job_available.wait(job_avail_guard, [this] { return isEOF || tasks.size() > 0; });
    std::lock_guard<std::mutex> pop_guard(slot_avail);
    std::lock_guard<std::mutex> queue_guard(queue_mutex);
    if (tasks.size() > 0) {
        int top = tasks.front();
        tasks.pop();
        is_slot_available.notify_all();
        return top;
    }
    return -1;
}

/** 
 * thread safe check for the size of the queue
 */
int ConcurrentQueue::Size() {
    std::lock_guard<std::mutex> size_guard(queue_mutex);
    return tasks.size();
}

/**
 * notifies any locked threads waiting for the queue to be filled to be released
 */
void ConcurrentQueue::NotifyConsumers(bool done) {
    isEOF = done;
    is_job_available.notify_all();
}
