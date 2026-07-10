#include <mana/mana.h>
#include <cassert>
#include <iostream>

void test_framework_initialize() {
    mana::initialize();

    // Should not crash
    auto& logger = mana::logger();
    logger.info("Framework initialized");

    mana::shutdown();
}

void test_framework_singleton() {
    mana::initialize();

    auto& instance1 = mana::instance();
    auto& instance2 = mana::instance();

    assert(&instance1 == &instance2);

    mana::shutdown();
}

void test_framework_modules() {
    mana::initialize();

    auto& pool = mana::instance().thread_pool;
    auto& bus = mana::instance().event_bus;
    auto& ctx = mana::instance().context;
    auto& logger = mana::logger();
    auto& fm = mana::file_manager();

    // Should not crash
    assert(pool.thread_count() > 0);

    mana::shutdown();
}
