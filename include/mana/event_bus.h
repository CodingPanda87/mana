#pragma once

#include <functional>
#include <memory>
#include <concepts>
#include <cstddef>
#include <any>
#include <typeindex>

#include "export.h"

namespace mana::event_bus {

// Event type concept - any copyable type can be used as an event
template<typename T>
concept EventType = std::copy_constructible<T>;

class MANA_API EventBus {
public:
    EventBus();
    ~EventBus();

    // Subscribe to event type
    template<EventType T>
    size_t subscribe(std::function<void(const T&)> callback) {
        auto wrapper = [cb = std::move(callback)](const std::any& event) {
            cb(std::any_cast<const T&>(event));
        };
        return subscribe_internal(std::type_index(typeid(T)), std::move(wrapper));
    }

    // Unsubscribe by ID
    template<EventType T>
    void unsubscribe(size_t subscription_id) {
        unsubscribe_internal(std::type_index(typeid(T)), subscription_id);
    }

    // Publish event asynchronously
    template<EventType T>
    void publish(const T& event) {
        std::any wrapped(event);
        publish_internal(std::type_index(typeid(T)), wrapped);
    }

    // Publish event synchronously (wait for all subscribers)
    template<EventType T>
    void publish_sync(const T& event) {
        std::any wrapped(event);
        publish_sync_internal(std::type_index(typeid(T)), wrapped);
    }

private:
    // Type-erased internal methods (implemented in .cpp)
    size_t subscribe_internal(std::type_index type, std::function<void(const std::any&)> callback);
    void unsubscribe_internal(std::type_index type, size_t subscription_id);
    void publish_internal(std::type_index type, const std::any& event);
    void publish_sync_internal(std::type_index type, const std::any& event);

    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace mana::event_bus
