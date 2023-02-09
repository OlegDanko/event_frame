#pragma once

#include "Event.hpp"
#include "Ticket.hpp"
#include "Protected.hpp"

#include <atomic>
#include <optional>
#include <unordered_map>
#include <memory>

size_t get_event_provider_id() {
    static std::atomic_size_t id{1};
    return id.fetch_add(1);
}

template<typename ...Params>
struct i_event_provider {
    using event_t = event<Params...>;
    using ret_t = std::optional<std::reference_wrapper<const event_t>>;
    virtual ret_t retrieve_event(const ticket& t) = 0;
};

template<typename ...Params>
struct event_spawner : i_ticket_tracker, i_event_provider<Params...> {
    using i_provider_t = i_event_provider<Params...>;
    using event_t = typename i_provider_t::event_t;
    Protected<std::unordered_map<size_t, std::unique_ptr<event_t>>> events_map;
    size_t id;

    event_spawner() : id(get_event_provider_id()) {}

    auto next_event_id() {
        static size_t event_id_counter{0};
        return ++event_id_counter;
    }

    std::shared_ptr<ticket> spawn_event(const Params& ...params) {
        auto event_id = next_event_id();

        auto locked_map = events_map.lock();
        locked_map.get()[event_id] = std::make_unique<event_t>(params...);
        return std::make_shared<ticket>(*this, event_id);
    }

    typename i_provider_t::ret_t retrieve_event(const ticket& t) override {
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
};
