#pragma once

#include <cinttypes>
#include <cstdio>
#include <limits>
#include <optional>
#include <string_view>
#include <variant>

#include <freertos_tracer_types.hpp>
#include <inplace_function.hpp>
#include <prohibit_copy_move.hpp>
#include <singleton.hpp>
#include <slab_allocator.hpp>
#include <span.hpp>

#include <FreeRTOS.h>
#include <task.h>

namespace event_tracer::freertos
{

/// @brief FreeRTOS event tracer implementation
class EventTracer : public ProhibitCopyMove
{
public:
    using get_time_cb_t = InplaceFunction<Event::timestamp_t()>;

    struct Settings
    {
        Span<std::byte> buff;
        uint max_tasks_expected;
        uint message_pool_capacity;
        data_ready_cb_t data_ready_cb;
        get_time_cb_t get_time_cb;
    };

    EventTracer(Settings settings);

    void register_event(EventId id, std::optional<TaskHandle_t> task = std::nullopt,
                        std::optional<Event::timestamp_t> timestamp = std::nullopt);

    void register_user_event(UserEventId id, std::optional<std::string_view> message = std::nullopt,
                             const TaskStatus_t *task_status = nullptr);

    [[nodiscard]] Event::timestamp_t now() const { return m_get_time_cb(); }

private:
    void on_registry_ready(EventRegistry &registry);
    void notify_done(EventRegistry &registry);

    constexpr bool needs_message(EventId id);

    EventRegistry *m_active_registry;
    EventRegistry *m_pending_registry;

    std::array<std::optional<EventRegistry>, 2> m_registries;

    data_ready_cb_t m_data_ready_cb;
    get_time_cb_t m_get_time_cb;

    uint64_t m_first_ts;

    const uint m_string_pool_capacity;
};

using SingleEventTracer = Singleton<EventTracer>;
using EventTracerPtr = SingleEventTracer::Ptr;

}  // namespace event_tracer::freertos
