#pragma once

#include <functional>
#include <future>
#include <memory>
#include <cstddef>
#include <thread>
#include <type_traits>

#include "export.h"

namespace mana::thread_pool {

class MANA_API ThreadPool {
public:
    explicit ThreadPool(size_t thread_count = std::thread::hardware_concurrency());
    ~ThreadPool();

    // Submit task and return future
    template<typename F, typename... Args>
    auto submit(F&& func, Args&&... args)
        -> std::future<std::invoke_result_t<F, Args...>>;

    // Wait for all pending tasks to complete
    void wait_all();

    // Get number of pending tasks
    size_t pending_count() const;

    // Get number of worker threads
    size_t thread_count() const;

private:
    // Internal submit with type-erased callable (defined in .cpp)
    void submit_internal(std::function<void()> task);

    class Impl;
    std::unique_ptr<Impl> impl_;
};

// Template implementation
template<typename F, typename... Args>
auto ThreadPool::submit(F&& func, Args&&... args)
    -> std::future<std::invoke_result_t<F, Args...>>
{
    using return_type = std::invoke_result_t<F, Args...>;

    auto task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(func), std::forward<Args>(args)...)
    );

    std::future<return_type> result = task->get_future();

    submit_internal([task]() { (*task)(); });

    return result;
}

} // namespace mana::thread_pool
