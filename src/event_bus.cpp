#include <mana/event_bus.h>
#include <unordered_map>
#include <vector>
#include <typeindex>
#include <shared_mutex>
#include <algorithm>
#include <any>

namespace mana::event_bus {

class EventBus::Impl {
public:
    struct Subscription {
        size_t id;
        std::function<void(const std::any&)> callback;
    };

    std::unordered_map<std::type_index, std::vector<Subscription>> subscribers;
    std::unordered_map<std::type_index, size_t> next_ids;
    mutable std::shared_mutex mutex;
};

EventBus::EventBus() : impl_(std::make_unique<Impl>()) {}

EventBus::~EventBus() = default;

size_t EventBus::subscribe_internal(std::type_index type, std::function<void(const std::any&)> callback) {
    std::unique_lock lock(impl_->mutex);
    size_t id = impl_->next_ids[type]++;
    impl_->subscribers[type].push_back({id, std::move(callback)});
    return id;
}

void EventBus::unsubscribe_internal(std::type_index type, size_t subscription_id) {
    std::unique_lock lock(impl_->mutex);
    auto it = impl_->subscribers.find(type);
    if (it != impl_->subscribers.end()) {
        auto& subs = it->second;
        subs.erase(
            std::remove_if(subs.begin(), subs.end(),
                [subscription_id](const Impl::Subscription& sub) {
                    return sub.id == subscription_id;
                }),
            subs.end()
        );
    }
}

void EventBus::publish_internal(std::type_index type, const std::any& event) {
    std::vector<std::function<void(const std::any&)>> callbacks;
    {
        std::shared_lock lock(impl_->mutex);
        auto it = impl_->subscribers.find(type);
        if (it != impl_->subscribers.end()) {
            for (const auto& sub : it->second) {
                callbacks.push_back(sub.callback);
            }
        }
    }
    for (const auto& cb : callbacks) {
        cb(event);
    }
}

void EventBus::publish_sync_internal(std::type_index type, const std::any& event) {
    publish_internal(type, event);
}

} // namespace mana::event_bus
