#pragma once

#include "Event.hpp"
#include <utils/Protected.hpp>
#include <utils/utils.hpp>

#include <atomic>
#include <optional>
#include <unordered_map>
#include <memory>

#include <utils/ticket_storage/TicketStorage.hpp>

using ticket_s_ptr_t = utl_prf::TicketDispenser::ticket_s_ptr_t;

size_t get_event_spawner_id() {
    static std::atomic_size_t id{1};
    return id.fetch_add(1);
}

template<typename ...Params>
struct IEventProvider {
    using event_t = event<Params...>;
    using ret_t = typename utl_prf::TicketStorage<const event_t>::ItemWrapper;
    virtual ret_t retrieve_event(ticket_s_ptr_t&) = 0;
    virtual size_t id() const = 0;
};

/**
 *  Creates and stores event objects and corresponding event tickets
 *  Provides event objects by corresponding event tickets
 *  Removes event objects when corresponding event tickets are deleted
 */
struct IEventSpawnListener {
    virtual void on_event_spawned(ticket_s_ptr_t&) = 0;
};

template<typename ...Params>
class EventSpawner : public IEventProvider<Params...> {
    using i_spawner_t = IEventProvider<Params...>;
    using event_t = typename i_spawner_t::event_t;

    utl_prf::TicketStorage<const event_t> storage;

//    utl_prf::Protected<std::unordered_map<size_t, std::unique_ptr<event_t>>> events_map;
    std::vector<IEventSpawnListener*> event_spawn_listeners;

    const size_t id_;

    auto next_event_id() {
        static size_t event_id_counter{0};
        return ++event_id_counter;
    }

    typename i_spawner_t::ret_t retrieve_event(ticket_s_ptr_t& ticket) override {
        return storage.get(ticket);
    }

public:
    EventSpawner() : id_(get_event_spawner_id()) {}
    EventSpawner(const EventSpawner&) = delete;
    EventSpawner(EventSpawner&&) = delete;

    void add_event_spawn_listener(IEventSpawnListener* listener) {
        event_spawn_listeners.push_back(listener);
    }

    void spawn_event(const Params& ...params) {
        auto ticket = storage.add(std::make_unique<event_t>(params...));

        for(auto listener : event_spawn_listeners) {
            listener->on_event_spawned(ticket);
        }
    }

    size_t id() const override {
        return id_;
    }
};
