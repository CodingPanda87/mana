#include <mana/log.h>
#include <cassert>
#include <iostream>
#include <sstream>

void test_log_level() {
    mana::log::Logger logger;
    logger.set_level(mana::log::Level::Info);
    assert(logger.level() == mana::log::Level::Info);
}

void test_log_filtering() {
    mana::log::Logger logger;
    logger.set_level(mana::log::Level::Warning);

    // These should not output (below threshold)
    logger.debug("Debug message");
    logger.info("Info message");

    // These should output
    logger.warning("Warning message");
    logger.error("Error message");
}

void test_log_format() {
    mana::log::Logger logger;
    logger.set_level(mana::log::Level::Debug);

    // Test formatted output
    logger.info("Hello {}, count: {}", "world", 42);
    logger.error("Error code: {}", 404);
}
