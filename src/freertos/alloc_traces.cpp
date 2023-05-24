#include <alloc_traces.h>
#include <event_tracer.hpp>
#include <event.hpp>

using namespace event_tracer::freertos;

extern "C" void vTraceMalloc(void *pvAddress, size_t uiSize)
{
    tracer().register_event(Event::MALLOC);
}

extern "C" void vTraceFree(void *pvAddress, size_t uiSize)
{
    tracer().register_event(Event::FREE);
}
