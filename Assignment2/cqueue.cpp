#include "cqueue.hpp"

ConcurrentQueue::ConcurrentQueue() {
    for (int i = 0; i < 10; i++) {
        this->tasks.push(i);
    }
}
ConcurrentQueue::~ConcurrentQueue() {}

void ConcurrentQueue::Push(int t) {
    this->tasks.push(t);
}

int ConcurrentQueue::Pop() {
    std::lock_guard<std::mutex> guard(this->tasksMutex);
    int top = this->tasks.front();
    this->tasks.pop();
    return top;
}

int ConcurrentQueue::Size() { return this->tasks.size(); }