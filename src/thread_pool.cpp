#include <mana/thread_pool.h>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <functional>

namespace mana::thread_pool {

class ThreadPool::Impl {
public:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;

    mutable std::mutex mutex;
    std::condition_variable condition;
    std::atomic<bool> stop{false};
    std::atomic<size_t> active_tasks{0};
};

ThreadPool::ThreadPool(size_t thread_count) : impl_(std::make_unique<Impl>()) {
    for (size_t i = 0; i < thread_count; i++) {
        impl_->workers.emplace_back([this]() {
            while (true) {
                std::function<void()> task;

                {
                    std::unique_lock<std::mutex> lock(impl_->mutex);
                    impl_->condition.wait(lock, [this]() {
                        return impl_->stop.load() || !impl_->tasks.empty();
                    });

                    if (impl_->stop.load() && impl_->tasks.empty()) {
                        return;
                    }

                    task = std::move(impl_->tasks.front());
                    impl_->tasks.pop();
                }

                impl_->active_tasks++;
                task();
                impl_->active_tasks--;
            }
        });
    }
}

ThreadPool::~ThreadPool() {
    impl_->stop.store(true);
    impl_->condition.notify_all();

    for (auto& worker : impl_->workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

void ThreadPool::submit_internal(std::function<void()> task) {
    {
        std::unique_lock<std::mutex> lock(impl_->mutex);
        impl_->tasks.push(std::move(task));
    }
    impl_->condition.notify_one();
}

void ThreadPool::wait_all() {
    while (true) {
        std::unique_lock<std::mutex> lock(impl_->mutex);
        if (impl_->tasks.empty() && impl_->active_tasks.load() == 0) {
            break;
        }
        lock.unlock();
        std::this_thread::yield();
    }
}

size_t ThreadPool::pending_count() const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    return impl_->tasks.size();
}

size_t ThreadPool::thread_count() const {
    return impl_->workers.size();
}

} // namespace mana::thread_pool
