#pragma once

#include "ITicketTracker.hpp"


struct ticket {
    i_ticket_tracker& tracker;
    size_t id;
    ticket(i_ticket_tracker& t, size_t id) : tracker(t), id(id) {}
    ~ticket() {
        tracker.on_ticket_destroyed(id);
    }
};
