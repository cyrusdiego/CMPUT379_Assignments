#include "../include/cqueue.hpp"

#include <iostream>

ConcurrentQueue::ConcurrentQueue(int size) : tasks(std::queue<int>()) {
    max_size = size;
}

ConcurrentQueue::~ConcurrentQueue() {}

void ConcurrentQueue::Push(int t) {
    std::unique_lock<std::mutex> push_guard(can_push);
    is_slot_available.wait(push_guard, [this] { return Size() < max_size; });
    std::lock_guard<std::mutex> pop_guard(can_pop);
    std::lock_guard<std::mutex> queue_guard(queue_mutex);
    tasks.push(t);
    is_job_available.notify_all();
}

int ConcurrentQueue::Pop() {
    std::unique_lock<std::mutex> pop_guard(can_pop);
    is_job_available.wait(pop_guard, [this] { return Size() > 0; });
    std::lock_guard<std::mutex> push_guard(can_push);
    std::lock_guard<std::mutex> queue_guard(queue_mutex);
    int top = tasks.front();
    tasks.pop();
    // only unlock the producer at queue is full then you take one out
    is_slot_available.notify_all();
    return top;
}

int ConcurrentQueue::Size() {
    return tasks.size();
}
