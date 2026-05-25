#include "../inc/task_queue.hpp"

void TaskQueue::push_task(std::function<void()> task) {
    std::lock_guard<std::mutex> lock(mutex_);
    tasks_.push(task);

    cv_.notify_one();
}

std::function<void()> TaskQueue::wait_and_pop() {
    std::unique_lock<std::mutex> lock(mutex_);

    cv_.wait(lock, [this]() {
        return stopped_ || !tasks_.empty();
    });

    if (stopped_ && tasks_.empty()) return nullptr;

    auto task = std::move(tasks_.front());
    tasks_.pop();

    return task;
}

void TaskQueue::stop() {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        stopped_ = true;
    }

    cv_.notify_all();
}
