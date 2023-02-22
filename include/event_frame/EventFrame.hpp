#pragma once

//#include "EventTicket.hpp"
#include <utils/ticket_storage/TicketStorage.hpp>

#include <unordered_map>
#include <unordered_set>
#include <memory>

using event_frame_t =
std::unordered_map<size_t, std::unordered_set<utl_prf::TicketDispenser::ticket_s_ptr_t>>;

struct IEventSubscriber {
    virtual void add_event_frame(const event_frame_t& frame) = 0;
    virtual size_t id() const = 0;
};

struct IEventChannel {
    virtual void add_event_subscriber(size_t spawner_id, IEventSubscriber* subscriber) = 0;
    virtual void add_ticket(size_t spawner_id, utl_prf::TicketDispenser::ticket_s_ptr_t t) = 0;
};
