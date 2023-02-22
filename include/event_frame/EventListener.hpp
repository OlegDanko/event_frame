#pragma once

#include "EventSpawner.hpp"

#include <functional>

template<typename ctx_t>
struct IEventListener {
    virtual size_t get_spawner_id() const = 0;
    virtual void serve_event(ticket_s_ptr_t& t, ctx_t& context) = 0;
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

    void serve_event(ticket_s_ptr_t& t, ctx_t& context) override {
        if(auto e_wrapper = spawner.retrieve_event(t); e_wrapper) {
            auto tpl = std::tuple_cat(std::tuple<ctx_t&>{context}, e_wrapper.unwrap().args);
            std::apply(callback, tpl);
        }
    }
};
