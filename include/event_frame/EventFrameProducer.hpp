#pragma once

#include <utils/utils.hpp>
#include "EventFrame.hpp"

class EventChannelBase : public IEventChannel {
    std::unordered_map<size_t, IEventSubscriber*> consumers_by_id;
    std::unordered_map<size_t, std::unordered_set<size_t>> spawner_id_to_consumer_id;
    std::unordered_map<size_t, event_frame_t> frame_by_consumer;
protected:
    void add_ticket_(size_t spawner_id, std::shared_ptr<event_ticket> t) {
        IF_PRESENT(spawner_id, spawner_id_to_consumer_id, consumers_set_it) {
            for(auto consumer_id : consumers_set_it->second) {
                frame_by_consumer[consumer_id][spawner_id].insert(t);
            }
        }
    }
    void flush_frame_buffers_() {
        for(auto& frame_it : frame_by_consumer) {
            IF_PRESENT(frame_it.first, consumers_by_id, consumer_it) {
                consumer_it->second->add_event_frame(frame_it.second);
            }
            frame_it.second = {};
        };
    }
public:
    void add_event_subscriber(size_t spawner_id, IEventSubscriber* subscriber) override {
        if(!utl_prf::is_present(subscriber->id(), consumers_by_id)) {
            consumers_by_id[subscriber->id()] = subscriber;
        }
        spawner_id_to_consumer_id[spawner_id].insert(subscriber->id());
    }
};

struct EventChannelCollector : public EventChannelBase {
    using base_t = EventChannelBase;
public:
    void add_ticket(size_t spawner_id, std::shared_ptr<event_ticket> t) override {
        base_t::add_ticket_(spawner_id, t);
    }
    void flush_frame_buffers() {
        base_t::flush_frame_buffers_();
    }
};

class EventChannelInstant : public EventChannelBase {
    using base_t = EventChannelBase;
public:
    void add_ticket(size_t spawner_id, std::shared_ptr<event_ticket> t) override {
        base_t::add_ticket_(spawner_id, t);
        base_t::flush_frame_buffers_();
    }
};
