#pragma once

#include "EventSpawner.hpp"

#include <functional>

template<typename ctx_t>
struct i_event_listener {
    virtual size_t get_spawner_id() const = 0;
    virtual void serve_event(const event_ticket& t, ctx_t& context) = 0;
};

template<typename ctx_t, typename ...Params>
class event_listener : public i_event_listener<ctx_t> {
    using spawner_t = i_event_provider<Params...>;
    spawner_t& spawner;
    using callback_t = std::function<void(ctx_t&, const Params&...)>;
    callback_t callback;
public:
    event_listener(spawner_t& s, callback_t cb) : spawner(s), callback(cb) {}

    size_t get_spawner_id() const override {
        return spawner.id();
    }

    void serve_event(const event_ticket& t, ctx_t& context) override {
        auto e = spawner.retrieve_event(t);
        if(!e.has_value()) {
            return;
        }
        auto tpl = std::tuple_cat(std::tuple<ctx_t&>{context}, e.value().get().args);
        std::apply(callback, tpl);
    }
};

using frame_t = std::unordered_map<size_t, event_ticket>;