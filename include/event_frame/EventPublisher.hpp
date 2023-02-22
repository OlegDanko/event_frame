#pragma once

#include "EventSpawner.hpp"
#include "EventFrame.hpp"
#include "EventListener.hpp"

template<typename ...Params>
struct EventPublisher : IEventSpawnListener {
    using event_spawner_t = EventSpawner<Params...>;
    event_spawner_t spawner;
    IEventChannel& channel;
    EventPublisher(IEventChannel& channel)
        : channel(channel) {
        spawner.add_event_spawn_listener(this);
    }

    void on_event_spawned(utl_prf::TicketDispenser::ticket_s_ptr_t& ticket) override {
        channel.add_ticket(spawner.id(), ticket);
    }

    template<typename ctx_t, typename ES, typename FN>
    void subscribe(ES& subscriber, FN callback) {
        subscriber.add_listener(
                    channel,
                    std::make_unique<EventListener<ctx_t, Params...>>(
                        spawner,
                        callback)
                    );
    }

    void operator()(const Params&... params) {
        spawner.spawn_event(params...);
    }

    EventPublisher(const EventPublisher&) = delete;
    EventPublisher(EventPublisher&&) = delete;
    EventPublisher& operator=(const EventPublisher&) = delete;
    EventPublisher& operator=(EventPublisher&&) = delete;
};
