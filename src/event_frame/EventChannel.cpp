#include <utils/utils.hpp>
#include <EventChannel.hpp>

void EventChannelBase::add_ticket_(size_t spawner_id, utl_prf::TicketDispenser::ticket_s_ptr_t t) {
    IF_PRESENT(spawner_id, spawner_id_to_subscriber_ids, subscriber_id_set_it) {
        for(auto subscriber_id : subscriber_id_set_it->second) {
            frame_by_subscriber_id[subscriber_id][spawner_id].insert(t);
        }
    }
}

void EventChannelBase::flush_frame_buffers_() {
    for(auto& frame_it : frame_by_subscriber_id) {
        IF_PRESENT(frame_it.first, subscribers_by_id, consumer_it) {
            consumer_it->second->add_event_frame(frame_it.second);
        }
        frame_it.second = {};
    };
}

void EventChannelBase::add_event_subscriber(size_t spawner_id, IEventSubscriber *subscriber) {
    if(!utl_prf::is_present(subscriber->id(), subscribers_by_id)) {
        subscribers_by_id[subscriber->id()] = subscriber;
    }
    spawner_id_to_subscriber_ids[spawner_id].insert(subscriber->id());
}

void EventChannelCollector::add_ticket(size_t spawner_id, utl_prf::TicketDispenser::ticket_s_ptr_t t) {
    base_t::add_ticket_(spawner_id, t);
}

void EventChannelCollector::flush_frame_buffers() {
    base_t::flush_frame_buffers_();
}

void EventChannelInstant::add_ticket(size_t spawner_id, ticket_s_ptr_t t) {
    base_t::add_ticket_(spawner_id, t);
    base_t::flush_frame_buffers_();
}
