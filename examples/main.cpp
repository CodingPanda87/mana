#include <mana/mana.h>
#include <mana/util/string.hpp>
#include <iostream>

int main() {
    // Initialize framework
    mana::initialize();

    // Direct module access
    auto& logger = mana::logger();
    logger.info("Mana framework initialized");

    // Use thread pool
    auto& pool = mana::instance().thread_pool;
    auto future = pool.submit([]() {
        return 42;
    });
    logger.info("Task result: {}", future.get());

    // Use event bus
    auto& bus = mana::instance().event_bus;

    struct MyEvent {
        int value;
    };

    bus.subscribe<MyEvent>([&logger](const MyEvent& e) {
        logger.info("Received event with value: {}", e.value);
    });

    MyEvent event{100};
    bus.publish_sync(event);

    // Use context via the thread-local context stack
    mana::context::Context ctx;
    ctx.put("username", std::string("panda"));
    mana::context::attach(std::move(ctx));

    auto& current_ctx = mana::context::current();
    if (auto username = current_ctx.get<std::string>("username")) {
        logger.info("Current user: {}", *username);
    }

    mana::context::detach();

    // Use utilities
    auto parts = mana::util::string::split("hello,world,foo", ",");
    logger.info("Split result: {} parts", parts.size());

    // Use file manager
    auto& fm = mana::file_manager();
    fm.write("test.txt", "Hello from Mana!");
    if (auto content = fm.read_all("test.txt")) {
        logger.info("File content: {}", *content);
    }

    // Shutdown framework
    mana::shutdown();
    return 0;
}
