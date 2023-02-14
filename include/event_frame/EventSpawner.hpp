#pragma once

#include "Event.hpp"
#include "EventTicket.hpp"
#include <utils/Protected.hpp>
#include <utils/utils.hpp>

#include <atomic>
#include <optional>
#include <unordered_map>
#include <memory>

size_t get_event_spawner_id() {
    static std::atomic_size_t id{1};
    return id.fetch_add(1);
}

template<typename ...Params>
struct IEventProvider {
    using event_t = event<Params...>;
    using ret_t = std::optional<std::reference_wrapper<const event_t>>;
    virtual ret_t retrieve_event(const EventTicket&) = 0;
    virtual size_t id() const = 0;
};

/**
 *  Creates and stores event objects and corresponding event tickets
 *  Provides event objects by corresponding event tickets
 *  Removes event objects when corresponding event tickets are deleted
 */
struct IEventSpawnListener {
    virtual void on_event_spawned(std::shared_ptr<EventTicket>&) = 0;
};

template<typename ...Params>
class EventSpawner : public IEventTicketTracker, public IEventProvider<Params...> {
    using i_spawner_t = IEventProvider<Params...>;
    using event_t = typename i_spawner_t::event_t;

    utl_prf::Protected<std::unordered_map<size_t, std::unique_ptr<event_t>>> events_map;
    std::vector<IEventSpawnListener*> event_spawn_listeners;

    const size_t id_;

    auto next_event_id() {
        static size_t event_id_counter{0};
        return ++event_id_counter;
    }

    typename i_spawner_t::ret_t retrieve_event(const EventTicket& ticket) override {
        auto locked_map = events_map.lock();

        IF_PRESENT(ticket.id, locked_map.get(), it) {
            return std::make_optional(std::cref(*it->second));
        }
        return std::nullopt;
    }

    void on_ticket_destroyed(size_t ticket_id) override {
        auto locked_map = events_map.lock();
        IF_PRESENT(ticket_id, locked_map.get(), it) {
            locked_map.get().erase(it);
        }
    }
public:
    EventSpawner() : id_(get_event_spawner_id()) {}

    void add_event_spawn_listener(IEventSpawnListener* listener) {
        event_spawn_listeners.push_back(listener);
    }

    void spawn_event(const Params& ...params) {
        auto event_id = next_event_id();

        with(auto locked_map = events_map.lock()) {
            locked_map.get()[event_id] = std::make_unique<event_t>(params...);
        }

        auto ticket = std::make_shared<EventTicket>(*this, event_id);

        for(auto listener : event_spawn_listeners) {
            listener->on_event_spawned(ticket);
        }
    }

    size_t id() const override {
        return id_;
    }
};
