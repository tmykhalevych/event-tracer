#pragma once

#include <freertos_tracer_types.hpp>
#include <inplace_function.hpp>
#include <prohibit_copy_move.hpp>
#include <singleton.hpp>
#include <slice.hpp>
#include <user_event_id.hpp>

#include <FreeRTOS.h>
#include <queue.h>
#include <task.h>

#include <optional>

namespace event_tracer::freertos
{

/// @brief RAII FreeRTOS event tracer client
/// @note Should be used as singleton ONLY
class Client : public ProhibitCopyMove
{
public:
    using get_timestamp_cb_t = InplaceFunction<uint64_t()>;

    struct ThreadSettings
    {
        task_prio_t prio = configTIMER_TASK_PRIORITY - 1;
        uint stack_size = configMINIMAL_STACK_SIZE;
        const char* name = "perf-tools";
        uint polling_interval_ms = 100;
    };

    struct Settings
    {
        Slice<std::byte> buff;
        get_timestamp_cb_t get_timestamp_cb;

        uint max_tasks_expected = 10;
        uint message_pool_capacity = 15;

        static constexpr auto THREADLESS = std::nullopt;

        std::optional<ThreadSettings> thread = ThreadSettings{};
    };

    Client(Settings settings, data_ready_cb_t consumer);
    ~Client();

    /// @brief Emits user event to register it alongside the FreeRTOS kernel events
    /// @param event Event emuneration
    /// @param message [optional] Message associated with an event
    void emit(UserEventId event, std::optional<std::string_view> message = std::nullopt);

    /// @brief Iterates through worker tasks, if client is created thread-less
    /// @note Useful if client is used together with external async engine/scheduler (e.g. Asio)
    /// @warning Asserts if client is thread-ful
    void iterate();

private:
    struct Message
    {
        EventRegistry* registry = nullptr;
        data_done_cb_t done_cb;
    };

    void iterate_impl();

    void client_task();
    void produce_message(Message&& msg);
    void dump_system_state();

    QueueHandle_t m_queue_hdl = nullptr;
    Slice<TaskStatus_t> m_system_state;

    TaskHandle_t m_task_hdl = nullptr;
    uint m_polling_interval_ms = 0;

    const data_ready_cb_t m_consumer;
    const uint m_max_tasks_expected;
    const bool m_threadless;
};

using SingleClient = Singleton<Client>;
using ClientPtr = SingleClient::Ptr;

/// @brief Event string formatter, converts event into "[ts|event_id|task|info_id:info]end"
/// @param e Event to format
/// @param end Last character, could be used to add newline or specific end value
/// @return Formatted event as a string_view
/// @warning Not MT-safe
[[nodiscard]] std::string_view format(const Event& e, char end = '\n');

/// @brief Event string formatter, converts event into "[ts|event_id|task|info_id:info]end"
/// @param dst Destination C-string
/// @param e Event to format
/// @param end Last character, could be used to add newline or specific end value
/// @return The number of characters written to destination or negative error code
/// @warning Not MT-safe
int format(char* dst, const Event& e, char end = '\n');

}  // namespace event_tracer::freertos
