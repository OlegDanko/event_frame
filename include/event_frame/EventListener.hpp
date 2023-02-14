#pragma once

#include "EventSpawner.hpp"

#include <functional>

template<typename ctx_t>
struct IEventListener {
    virtual size_t get_spawner_id() const = 0;
    virtual void serve_event(const EventTicket& t, ctx_t& context) = 0;
};

template<typename ctx_t, typename ...Params>
class EventListener : public IEventListener<ctx_t> {
    using spawner_t = IEventProvider<Params...>;
    spawner_t& spawner;
    using callback_t = std::function<void(ctx_t&, const Params&...)>;
    callback_t callback;
public:
    EventListener(spawner_t& s, callback_t cb) : spawner(s), callback(cb) {}

    size_t get_spawner_id() const override {
        return spawner.id();
    }

    void serve_event(const EventTicket& t, ctx_t& context) override {
        auto e = spawner.retrieve_event(t);
        if(!e.has_value()) {
            return;
        }
        auto tpl = std::tuple_cat(std::tuple<ctx_t&>{context}, e.value().get().args);
        std::apply(callback, tpl);
    }
};

using frame_t = std::unordered_map<size_t, EventTicket>;
