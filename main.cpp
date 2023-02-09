#include "EventListener.hpp"

#include <iostream>

int main()
{
    event_spawner<int, int, int> event_spawner_obj;

    event_listener<int, int, int, int> event_listener_obj(
                event_spawner_obj,
                [](int& ctx,
                const int& a,
                const int& b,
                const int& c) {
        std::cout << "ctx = " << ++ctx
                  << ", params: "
                  << a << ", "
                  << b << ", "
                  << c << "\n";
    });

    auto t = event_spawner_obj.spawn_event(10, 20, 30);
    int ctx{0};
    event_listener_obj.serve_event(*t, ctx);
    event_listener_obj.serve_event(*t, ctx);

    return 0;
}
