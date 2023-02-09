#include <EventFrameConsumer.hpp>
#include <EventFrameProducer.hpp>
#include <iostream>


int main()
{
    // Event source
    EventFrameProducer test_producer;
    event_spawner<int, int, int> test_spawner;

    // Event targer
    std::string context_1 = "consumer 1";
    EventFrameConsumer<std::string> test_consumer_1;
    std::string context_2 = "consumer 2";
    EventFrameConsumerInstant<std::string> test_consumer_2(context_2);

    // Bind listener
    test_consumer_1.add_listener(test_producer,
                                 std::make_unique<event_listener<std::string, int, int, int>>(
                                     test_spawner,
                                     [](std::string& ctx,
                                     const int& a,
                                     const int& b,
                                     const int& c) {
                             std::cout << "ctx = " << ctx
                                       << ", params: "
                                       << a << ", "
                                       << b << ", "
                                       << c << "\n";
                         }));
    test_consumer_2.add_listener(test_producer,
                                 std::make_unique<event_listener<std::string, int, int, int>>(
                                     test_spawner,
                                     [](std::string& ctx,
                                     const int& a,
                                     const int& b,
                                     const int& c) {
                             std::cout << "ctx = " << ctx
                                       << ", params: "
                                       << a << " <- a, "
                                       << b << " <- b, "
                                       << c << " <- c\n";
                         }));

    // Create event
    test_producer.add_ticket(test_spawner.id(), test_spawner.spawn_event(10, 20, 30));
    test_producer.flush_frame_buffers();

    // Consume event
    test_consumer_1.serve_frames(context_1);

    test_producer.add_ticket(test_spawner.id(), test_spawner.spawn_event(100, 200, 300));
    test_producer.flush_frame_buffers();

    test_producer.add_ticket(test_spawner.id(), test_spawner.spawn_event(1000, 2000, 3000));
    test_producer.flush_frame_buffers();


    // Consume event
    test_consumer_1.serve_frames(context_1);

    return 0;
}
