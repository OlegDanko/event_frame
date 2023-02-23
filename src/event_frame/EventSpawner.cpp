#include <EventSpawner.hpp>

size_t get_event_spawner_id() {
    static std::atomic_size_t id{1};
    return id.fetch_add(1);
}
