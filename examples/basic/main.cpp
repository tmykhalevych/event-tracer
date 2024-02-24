#include <event_registry.hpp>
#include <slice.hpp>

#include <array>
#include <cinttypes>
#include <iostream>

using Event = event_tracer::Event<>;
using Registry = event_tracer::EventRegistry<Event>;
using event_tracer::Slice;

int main()
{
    std::vector<Event> storage(4);
    Registry registry(as_slice(storage));

    registry.set_ready_cb([&registry](auto& events) {
        std::cout << "Dumping events : ";
        for (auto& event : events) {
            std::cout << std::to_string(event.id);
        }
        std::cout << '\n';
        registry.reset();
    });

    for (uint8_t i = 0; i < 10; ++i) {
        registry.add({.ts = i, .id = i});
    }

    return 0;
}
