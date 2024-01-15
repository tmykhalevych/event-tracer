#pragma once

#include <freertos_tracer_types.hpp>
#include <inplace_function.hpp>
#include <prohibit_copy_move.hpp>
#include <singleton.hpp>
#include <span.hpp>
#include <user_event_id.hpp>

#include <FreeRTOS.h>
#include <queue.h>
#include <task.h>
 
namespace freertos_tracer = event_tracer::freertos;

namespace event_tracer::freertos
{

/// @brief RAII FreeRTOS event tracer client
/// @note Should be used as singleton ONLY
class Client : public ProhibitCopyMove
{
public:
    using get_timestamp_cb_t = InplaceFunction<uint64_t()>;

    struct Settings
    {
        Span<std::byte> buff;
        get_timestamp_cb_t get_timestamp_cb;
        uint polling_interval_ms = 100;

        task_prio_t prio = configTIMER_TASK_PRIORITY - 1;
        uint stack_size = configMINIMAL_STACK_SIZE;
        const char* name = "perf-tools";
    };

    Client(Settings settings, data_ready_cb_t consumer);
    ~Client();

    /// @brief Emits user event to register it alongside the FreeRTOS kernel events
    /// @param event Event emuneration
    /// @param message [optional] Message associated with an event
    void emit(UserEventId event, std::optional<std::string_view> message = std::nullopt);

private:
    struct Message
    {
        EventRegistry* registry = nullptr;
        data_done_cb_t done_cb;
    };

    void client_task();
    void produce_message(Message&& msg);

    QueueHandle_t m_queue_hdl = nullptr;
    TaskHandle_t m_task_hdl = nullptr;

    const data_ready_cb_t m_consumer;
    const uint m_polling_interval_ms;
};

using SingleClient = Singleton<Client>;
using ClientPtr = SingleClient::Ptr;

/// @brief Event string formatter
/// @param event Event to format
/// @param newline Indicator for adding newline character at the end of string
/// @return Formatted event as a string_view
[[nodiscard]] std::string_view format(const Event& e, bool newline = true);

}  // namespace event_tracer::freertos
