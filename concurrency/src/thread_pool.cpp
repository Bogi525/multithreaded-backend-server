#include "../inc/thread_pool.hpp"

ThreadPool::ThreadPool(size_t thread_count) {
    for (size_t i = 0; i < thread_count; i++) {
        workers_.emplace_back([this] () {
            while (true) {
                auto task = task_queue_.wait_and_pop();

                if (task == nullptr) break;

                task();
            }
        });
    }
}