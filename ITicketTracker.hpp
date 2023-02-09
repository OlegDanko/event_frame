#pragma once
#include <stddef.h>

struct i_ticket_tracker {
    virtual void on_ticket_destroyed(size_t id) = 0;
};
