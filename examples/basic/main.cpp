#include <cinttypes>
#include <event_registry.hpp>
#include <iostream>

struct TaskContext
{
    uint8_t id;
    uint8_t prio;
};

using TaskEventDesc = event_tracer::Event<TaskContext>;

int main()
{
    event_tracer::EventRegistry<TaskEventDesc> registry(4);

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
