#pragma once
#include <stddef.h>

struct IEventTicketTracker {
    virtual void on_ticket_destroyed(size_t id) = 0;
};

struct EventTicket {
    IEventTicketTracker& tracker;
    const size_t id;
    EventTicket(IEventTicketTracker& t, size_t id) : tracker(t), id(id) {}
    ~EventTicket() {
        tracker.on_ticket_destroyed(id);
    }
    EventTicket(const EventTicket&) = delete;
    EventTicket(EventTicket&&) = delete;
    EventTicket& operator=(const EventTicket&) = delete;
    EventTicket& operator=(EventTicket&&) = delete;
};
