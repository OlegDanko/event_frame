#pragma once

#include "EventProvider.hpp"

#include <functional>

template<typename ctx_t, typename ...Params>
struct event_listener {
    using provider_t = i_event_provider<Params...>;
    provider_t& provider;
    using callback_t = std::function<void(ctx_t&, const Params&...)>;
    callback_t callback;

    event_listener(provider_t& s, callback_t cb) : provider(s), callback(cb) {}

    void serve_event(const ticket& t, ctx_t& context) {
        auto e = provider.retrieve_event(t);
        if(!e.has_value()) {
            return;
        }
        auto tpl = std::tuple_cat(std::tuple<ctx_t&>{context}, e.value().get().args);
        std::apply(callback, tpl);
    }
};

using frame_t = std::unordered_map<size_t, ticket>;
