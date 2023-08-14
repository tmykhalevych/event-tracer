#include <alloc_traces.h>
#include <event.hpp>
#include <event_tracer.hpp>

using namespace event_tracer::freertos;

extern "C" void vTraceMalloc([[maybe_unused]] void *pvAddress, [[maybe_unused]] size_t uiSize)
{
    tracer().register_event(Event::MALLOC);
}

extern "C" void vTraceFree([[maybe_unused]] void *pvAddress, [[maybe_unused]] size_t uiSize)
{
    tracer().register_event(Event::FREE);
}
