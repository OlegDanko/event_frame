#pragma once

#include <utils/utils.hpp>
#include "EventFrame.hpp"

class EventChannelBase : public IEventChannel {
    std::unordered_map<size_t, IEventSubscriber*> subscribers_by_id;
    std::unordered_map<size_t, std::unordered_set<size_t>> spawner_id_to_subscriber_ids;
    std::unordered_map<size_t, event_frame_t> frame_by_subscriber_id;
protected:
    void add_ticket_(size_t spawner_id, ticket_s_ptr_t t);
    void flush_frame_buffers_();
public:
    void add_event_subscriber(size_t spawner_id, IEventSubscriber* subscriber) override;
};

struct EventChannelCollector : public EventChannelBase {
    using base_t = EventChannelBase;
public:
    void add_ticket(size_t spawner_id, ticket_s_ptr_t t) override;
    void flush_frame_buffers();
};

class EventChannelInstant : public EventChannelBase {
    using base_t = EventChannelBase;
public:
    void add_ticket(size_t spawner_id, ticket_s_ptr_t t) override;
};
