#pragma once
#include <stddef.h>

struct i_event_ticket_tracker {
    virtual void on_ticket_destroyed(size_t id) = 0;
};

struct event_ticket {
    i_event_ticket_tracker& tracker;
    const size_t id;
    event_ticket(i_event_ticket_tracker& t, size_t id) : tracker(t), id(id) {}
    ~event_ticket() {
        tracker.on_ticket_destroyed(id);
    }
    event_ticket(const event_ticket&) = delete;
    event_ticket(event_ticket&&) = delete;
    event_ticket& operator=(const event_ticket&) = delete;
    event_ticket& operator=(event_ticket&&) = delete;
};
