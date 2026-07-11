#include <mana/event.h>
#include <cassert>
#include <string>

struct TestEvent {
    int value;
};

struct StringEvent {
    std::string value;
};

void test_event_subscribe_publish() {
    mana::event::EventBus bus;
    int received_value = 0;

    bus.subscribe<TestEvent>([&received_value](const TestEvent& e) {
        received_value = e.value;
    });

    bus.publish_sync(TestEvent{42});
    assert(received_value == 42);
}

void test_event_multiple_subscribers() {
    mana::event::EventBus bus;
    int count = 0;

    bus.subscribe<TestEvent>([&count](const TestEvent&) {
        count++;
    });

    bus.subscribe<TestEvent>([&count](const TestEvent&) {
        count++;
    });

    bus.publish_sync(TestEvent{0});
    assert(count == 2);
}

void test_event_unsubscribe() {
    mana::event::EventBus bus;
    int count = 0;

    auto id = bus.subscribe<TestEvent>([&count](const TestEvent&) {
        count++;
    });

    bus.publish_sync(TestEvent{0});
    assert(count == 1);

    bus.unsubscribe<TestEvent>(id);
    bus.publish_sync(TestEvent{0});
    assert(count == 1);  // Should not increase
}

void test_event_different_types() {
    mana::event::EventBus bus;
    int int_value = 0;
    std::string str_value;

    bus.subscribe<TestEvent>([&int_value](const TestEvent& e) {
        int_value = e.value;
    });

    bus.subscribe<StringEvent>([&str_value](const StringEvent& e) {
        str_value = e.value;
    });

    bus.publish_sync(TestEvent{42});
    bus.publish_sync(StringEvent{"hello"});

    assert(int_value == 42);
    assert(str_value == "hello");
}
