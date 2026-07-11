#include <mana/mana.h>
#include <cassert>
#include <cmath>
#include <iostream>

void test_framework_initialize() {
    mana::initialize();
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
    auto& bus = mana::instance().event;
    auto& ctx = mana::instance().context;
    auto& logger = mana::logger();
    auto& pool = mana::instance().thread_pool;
    assert(pool.thread_count() > 0);
    mana::shutdown();
}

void test_util_modules() {
    // Test string utilities
    auto result = mana::util::string::split("hello,world", ",");
    assert(result.size() == 2);

    // Test math utilities
    assert(mana::util::math::clamp(5, 1, 10) == 5);

    // Test geo utilities
    mana::util::geo::Point p1{0, 0};
    mana::util::geo::Point p2{3, 4};
    assert(std::abs(p1.distance_to(p2) - 5.0) < 1e-10);
}
