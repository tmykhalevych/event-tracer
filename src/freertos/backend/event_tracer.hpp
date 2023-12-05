#pragma once

#include <cinttypes>
#include <cstdio>
#include <limits>
#include <optional>
#include <string_view>
#include <variant>

#include <common.hpp>
#include <inplace_function.hpp>
#include <prohibit_copy_move.hpp>
#include <singleton.hpp>
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

    EventTracer(Span<std::byte> buff, data_ready_cb_t data_ready_cb, get_time_cb_t get_time_cb);

    void register_event(EventId id, std::optional<TaskHandle_t> task = std::nullopt,
                        std::optional<Event::timestamp_t> timestamp = std::nullopt);

    void register_user_event(const message_t &message);

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
};

using SingleEventTracer = Singleton<EventTracer>;
using EventTracerPtr = SingleEventTracer::Ptr;

}  // namespace event_tracer::freertos
