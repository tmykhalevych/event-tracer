#include <event_registry.hpp>
#include <slice.hpp>

#include <array>
#include <cinttypes>
#include <iostream>

struct TaskContext
{
    uint8_t id;
    uint8_t prio;
};

using TaskEventDesc = event_tracer::Event<TaskContext>;

int main()
{
    std::array<TaskEventDesc, 4> storage;

    event_tracer::EventRegistry<TaskEventDesc> registry(event_tracer::Slice(storage.data(), storage.size()));

    registry.set_ready_cb([](auto& events) {
        std::cout << "Dumping events : ";

        for (auto& event : events) std::cout << std::to_string(event.id);

        std::cout << std::endl;
    });

    for (uint8_t i = 0; i < 10; ++i) {
        registry.add({.id = i});
    }

    return 0;
}
