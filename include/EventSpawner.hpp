#pragma once

#include "Event.hpp"
#include "EventTicket.hpp"
#include "Protected.hpp"

#include <atomic>
#include <optional>
#include <unordered_map>
#include <memory>

size_t get_event_spawner_id() {
    static std::atomic_size_t id{1};
    return id.fetch_add(1);
}

template<typename ...Params>
struct i_event_provider {
    using event_t = event<Params...>;
    using ret_t = std::optional<std::reference_wrapper<const event_t>>;
    virtual ret_t retrieve_event(const event_ticket& t) = 0;
    virtual size_t id() const = 0;
};

/**
 *  Creates and stores event objects and corresponding event tickets
 *  Provides event objects by corresponding event tickets
 *  Removes event objects when corresponding event tickets are deleted
 */
template<typename ...Params>
class event_spawner : public i_event_ticket_tracker, public i_event_provider<Params...> {
    using i_spawner_t = i_event_provider<Params...>;
    using event_t = typename i_spawner_t::event_t;

    Protected<std::unordered_map<size_t, std::unique_ptr<event_t>>> events_map;
    const size_t id_;

    auto next_event_id() {
        static size_t event_id_counter{0};
        return ++event_id_counter;
    }

    typename i_spawner_t::ret_t retrieve_event(const event_ticket& t) override {
        auto locked_map = events_map.lock();
        auto it = locked_map.get().find(t.id);
        return locked_map.get().end() != it
            ? std::make_optional(std::cref(*it->second))
            : std::nullopt;
    }

    void on_ticket_destroyed(size_t ticket_id) override {
        auto locked_map = events_map.lock();
        if(auto it = locked_map.get().find(ticket_id); locked_map.get().end() != it) {
            locked_map.get().erase(it);
        }
    }
public:
    event_spawner() : id_(get_event_spawner_id()) {}

    std::shared_ptr<event_ticket> spawn_event(const Params& ...params) {
        auto event_id = next_event_id();

        auto locked_map = events_map.lock();
        locked_map.get()[event_id] = std::make_unique<event_t>(params...);
        return std::make_shared<event_ticket>(*this, event_id);
    }

    size_t id() const override {
        return id_;
    }
};
