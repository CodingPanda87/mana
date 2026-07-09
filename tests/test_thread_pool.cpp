#include <mana/thread_pool.h>
#include <cassert>
#include <iostream>
#include <future>
#include <vector>
#include <atomic>

void test_thread_pool_submit() {
    mana::thread_pool::ThreadPool pool(2);

    auto future = pool.submit([]() {
        return 42;
    });

    assert(future.get() == 42);
}

void test_thread_pool_multiple_tasks() {
    mana::thread_pool::ThreadPool pool(2);

    std::vector<std::future<int>> futures;
    for (int i = 0; i < 10; i++) {
        futures.push_back(pool.submit([i]() {
            return i * 2;
        }));
    }

    for (int i = 0; i < 10; i++) {
        assert(futures[i].get() == i * 2);
    }
}

void test_thread_pool_wait_all() {
    mana::thread_pool::ThreadPool pool(2);

    std::atomic<int> counter{0};
    for (int i = 0; i < 5; i++) {
        pool.submit([&counter]() {
            counter++;
        });
    }

    pool.wait_all();
    assert(counter == 5);
}

void test_thread_pool_pending_count() {
    mana::thread_pool::ThreadPool pool(1);

    // Submit a blocking task
    std::promise<void> blocker;
    pool.submit([&blocker]() {
        blocker.get_future().wait();
    });

    // Submit more tasks
    pool.submit([]() {});
    pool.submit([]() {});

    assert(pool.pending_count() >= 2);

    // Release blocker
    blocker.set_value();
    pool.wait_all();
}
