#pragma once

#include "EventFrame.hpp"

struct EventsFrameProducer : IEventFrameProducer {
    std::unordered_map<size_t, IEventFrameConsumer*> consumers_by_id;
    std::unordered_map<size_t, std::unordered_set<size_t>> spawner_id_to_consumer_id;

    // each consumer has a dedicated frame for collecting events
    std::unordered_map<size_t, event_frame_t> frame_by_consumer;

    void add_event_frame_consumer(size_t spawner_id, IEventFrameConsumer* consumer) {
        if(consumers_by_id.find(consumer->id()) == consumers_by_id.end())
            consumers_by_id[consumer->id()] = consumer;
        spawner_id_to_consumer_id[spawner_id].insert(consumer->id());
    }

    void add_ticket(size_t spawner_id, std::shared_ptr<event_ticket> t) {
        auto consumers_set_it = spawner_id_to_consumer_id.find(spawner_id);
        if(spawner_id_to_consumer_id.end() == consumers_set_it) {
            return;
        }
        for(auto consumer_id : consumers_set_it->second) {
            frame_by_consumer[consumer_id][spawner_id].insert(t);
        }
    }
    void flush_frame_buffers() {
        for(auto& frame_it : frame_by_consumer) {
            auto consumer_it = consumers_by_id.find(frame_it.first);
            if(consumers_by_id.end() == consumer_it) {
                continue;
            }
            consumer_it->second->add_event_frame(frame_it.second);
            frame_it.second = {};
        };
    }
};
