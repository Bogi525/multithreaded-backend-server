#ifndef _thread_pool_hpp_
#define _thread_pool_hpp_

#include "task_queue.hpp"
#include <thread>

class ThreadPool {
public:
    ThreadPool(size_t thread_count);
    ~ThreadPool(); // Rule 3/5 !

    void enqueue(std::function<void()> task);

private:
    std::vector<std::thread> workers_;
    TaskQueue task_queue_;
};

#endif