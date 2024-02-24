#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>

#include <event_tracer_client.hpp>

#include <array>
#include <cassert>
#include <chrono>
#include <csignal>
#include <cstdarg>
#include <iostream>
#include <random>
#include <string_view>
#include <utility>

// clang-format off
static constexpr auto TASK_PRIO = configTIMER_TASK_PRIORITY - 2;
static const     auto TASK_STACK_SIZE = configMINIMAL_STACK_SIZE;
static constexpr auto TASK_NUM = 30;
static constexpr auto TASK_NAME_BASE = "task #";
static constexpr auto TASK_SLEEP_RANGE_MS = std::make_pair(100, 500);
static constexpr auto TRACES_BUFF_LEN = 0x2800;  // 10K
// clang-format on

void start_event_capturing_for(std::chrono::seconds duration);
void post_message_in(std::chrono::seconds duration, std::string msg);
// FreeRTOS hooks
extern "C" void vApplicationMallocFailedHook();
extern "C" void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName);
extern "C" void vAssertCalled(const char *const pcFileName, unsigned long ulLine);

int main()
{
    // successful exit is needed for gprof to generate gmon.out file
    signal(SIGINT, [](int) { exit(EXIT_SUCCESS); });

    std::array<std::byte, TRACES_BUFF_LEN> traces_buff;

    auto get_steady_time = []() -> uint64_t {
        using namespace std::chrono;
        return duration_cast<microseconds>(steady_clock::now().time_since_epoch()).count();
    };

    auto consume_traces = [](freertos_tracer::EventRegistry &registry, freertos_tracer::data_done_cb_t done_cb) {
        for (const auto &event : registry) {
            std::cout << freertos_tracer::format(event, '\n');
        }
        done_cb();
    };

    freertos_tracer::SingleClient::emplace(
        freertos_tracer::Client::Settings{.buff = event_tracer::Slice(traces_buff.data(), traces_buff.size()),
                                          .get_timestamp_cb = std::move(get_steady_time),
                                          .max_tasks_expected = 32,
                                          .message_pool_capacity = 65},
        std::move(consume_traces));

    const auto task = [](void *) {
        std::random_device dev;
        std::mt19937 rng(dev());
        std::uniform_int_distribution<std::mt19937::result_type> sleep_dist(TASK_SLEEP_RANGE_MS.first,
                                                                            TASK_SLEEP_RANGE_MS.second);

        const auto task_sleep_ms = sleep_dist(rng);
        while (true) {
            auto *buff = pvPortMalloc(1000);
            std::cout << pcTaskGetName(nullptr) << '\n';
            vPortFree(buff);
            vTaskDelay(pdMS_TO_TICKS(task_sleep_ms));
        }
    };

    for (int i = 1; i <= TASK_NUM; ++i) {
        const auto task_name = std::string(TASK_NAME_BASE) + std::to_string(i);
        xTaskCreate(task, task_name.c_str(), TASK_STACK_SIZE, nullptr, TASK_PRIO, nullptr);
    }

    using namespace std::chrono_literals;

    start_event_capturing_for(20s);
    post_message_in(1s, "message");

    vTaskStartScheduler();
    return 0;
}

void start_event_capturing_for(std::chrono::seconds duration)
{
    using freertos_tracer::UserEventId;

    freertos_tracer::SingleClient::instance()->emit(UserEventId::START_CAPTURING, "test_app");

    auto capturing_timer = xTimerCreate(
        "CapturingTimer", pdMS_TO_TICKS(duration.count() * 1000), pdFALSE, 0,
        [](TimerHandle_t) { freertos_tracer::SingleClient::instance()->emit(UserEventId::STOP_CAPTURING); });

    if (!capturing_timer) return;
    xTimerStart(capturing_timer, 0);
}

void post_message_in(std::chrono::seconds duration, std::string msg)
{
    using freertos_tracer::UserEventId;

    static std::string message = std::move(msg);

    auto capturing_timer = xTimerCreate(
        "MessageTimer", pdMS_TO_TICKS(duration.count() * 1000), pdFALSE, 0,
        [](TimerHandle_t) { freertos_tracer::SingleClient::instance()->emit(UserEventId::MESSAGE, message); });

    if (!capturing_timer) return;
    xTimerStart(capturing_timer, 0);
}

extern "C" void vApplicationMallocFailedHook()
{
    std::cerr << "malloc failed" << std::endl;
    assert(false);
}

extern "C" void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName)
{
    std::cerr << "stack overflow, task: \"" << std::string_view(pcTaskName) << "\"" << std::endl;
    assert(false);
}

extern "C" void vAssertCalled(const char *const pcFileName, unsigned long ulLine)
{
    std::cerr << "assert at " << std::string_view(pcFileName) << ":" << ulLine << std::endl;
    assert(false);
}
