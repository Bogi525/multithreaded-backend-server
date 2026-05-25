#ifndef _task_queue_hpp_
#define _task_queue_hpp_

#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>

class TaskQueue {
public:
    void push_task(std::function<void()> task);
    std::function<void()> wait_and_pop();
    void stop();
private:
    std::queue<std::function<void()>> tasks_;

    std::mutex mutex_;
    std::condition_variable cv_;
    bool stopped_ = false;
};

#endif
