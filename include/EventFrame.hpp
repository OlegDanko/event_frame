#pragma once

#include "EventTicket.hpp"

#include <unordered_map>
#include <unordered_set>
#include <memory>

using event_frame_t = std::unordered_map<size_t, std::unordered_set<std::shared_ptr<event_ticket>>>;

struct IEventFrameConsumer {
    virtual void add_event_frame(const event_frame_t& frame) = 0;
    virtual size_t id() const = 0;
};

struct IEventFrameProducer {
    virtual void add_event_frame_consumer(size_t spawner_id, IEventFrameConsumer* consumer) = 0;
};
