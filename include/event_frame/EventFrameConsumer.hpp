#pragma once

#include "EventFrame.hpp"
#include "EventListener.hpp"
#include <atomic>
#include <queue>

struct EventFrameConsumerBase : IEventFrameConsumer {
    static size_t next_id() {
        static std::atomic_size_t id{1};
        return id.fetch_add(1);
    }
    const size_t id_;
    EventFrameConsumerBase() : id_(next_id()) {}
    size_t id() const override { return id_; }
};

template<typename ctx_t>
struct EventFrameConsumer : EventFrameConsumerBase {
    using event_listener_ptr_t = std::unique_ptr<i_event_listener<ctx_t>>;
    std::unordered_map<size_t, std::unordered_set<event_listener_ptr_t>> listeners_by_spawner_id;
    utl_prf::Protected<std::queue<event_frame_t>> frames;

    void add_listener(IEventFrameProducer& producer, event_listener_ptr_t listener) {
        producer.add_event_frame_consumer(listener->get_spawner_id(), this);
        listeners_by_spawner_id[listener->get_spawner_id()].insert(std::move(listener));
    }

    void add_event_frame(const event_frame_t& frame) override {
        frames.lock().get().push(frame);
    }

    void serve_event_ticket(size_t spawner_id, event_ticket& t, ctx_t& context) {
        IF_PRESENT(spawner_id, listeners_by_spawner_id, listeners_it){
            for(auto& l : listeners_it->second) {
                l->serve_event(t, context);
            }
        }
    }

    void serve_frame(event_frame_t& frame, ctx_t& context) {
        for(auto& spawner_tickets_it : frame) {
            for(auto& ticket : spawner_tickets_it.second) {
                serve_event_ticket(spawner_tickets_it.first, *ticket, context);
            }
        }
    }

    auto fetch_frames() {
        auto frames_locked = frames.lock();
        auto f = std::move(frames_locked.get());
        frames_locked.get() = {};
        return f;
    }

    void serve_frames(ctx_t& context) {
        auto frames_fetched = fetch_frames();
        while(!frames_fetched.empty()) {
            serve_frame(frames_fetched.front(), context);
            frames_fetched.pop();
        }
    }
};
