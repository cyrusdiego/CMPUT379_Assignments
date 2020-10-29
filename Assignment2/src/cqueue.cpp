#include "../include/cqueue.hpp"

#include <iostream>
// https://stackoverflow.com/questions/50331130/please-explain-the-use-of-condition-variables-in-c-threads-and-why-do-we-need/50347715#50347715
// explains you don't need to wrap wait with while loop

ConcurrentQueue::ConcurrentQueue(int size) : tasks(), max_size(size), isEOF(false) {}

ConcurrentQueue::~ConcurrentQueue() {}

void ConcurrentQueue::Push(int t) {
    std::lock_guard<std::mutex> push_guard(job_avail);
    std::unique_lock<std::mutex> slot_avail_guard(slot_avail);
    is_slot_available.wait(slot_avail_guard, [this] { return tasks.size() < max_size; });
    std::lock_guard<std::mutex> queue_guard(queue_mutex);
    tasks.push(t);
    is_job_available.notify_all();
}

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

int ConcurrentQueue::Size() {
    return tasks.size();
}

void ConcurrentQueue::NotifyConsumers(bool done) {
    isEOF = done;
    is_job_available.notify_all();
}
