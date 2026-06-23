#include "../inc/thread_pool.hpp"

#include <iostream>
#include "../../logging/inc/logger.hpp"

thread_local int ThreadPool::worker_id = -1;

int ThreadPool::current_worker_id() {
    return worker_id;
}

ThreadPool::ThreadPool(size_t thread_count) {
    for (size_t i = 0; i < thread_count; i++) {
        worker_threads_.emplace_back([this, i] () {
            worker_id = static_cast<int>(i);

            Logger::info("Worker ", i, " started");

            while (true) {
                auto task = task_queue_.wait_and_pop();

                if (!task) break;

                try {
                    task();
                } catch (std::exception e) {
                    Logger::error("Worker ", current_worker_id, " caught exception: ", e.what());
                } catch (...) {
                    Logger::error("Worker ", current_worker_id, " caught unknown exception");
                }
            }

            Logger::info("Worker ", i, " exiting");
        });
    }
}

ThreadPool::~ThreadPool() {
    task_queue_.stop();

    for (auto& worker: worker_threads_) {
        worker.join();
    }
}

void ThreadPool::enqueue(std::function<void()> task) {
    task_queue_.push_task(std::move(task));
}

