#include <FreeRTOS.h>
#include <task.h>

#include <event_tracer_client.hpp>

#include <array>
#include <cassert>
#include <chrono>
#include <cstdarg>
#include <iostream>
#include <random>
#include <string_view>
#include <utility>

static constexpr auto MAX_TASK_PRIORITY = configTIMER_TASK_PRIORITY - 2;
static const auto MIN_TASK_STACK_SIZE = configMINIMAL_STACK_SIZE;

static constexpr std::pair<int, int> TASK_SLEEP_RANGE_MS = {100, 500};
static constexpr auto TASK_NUM = 5;
static constexpr auto TASK_NAME_BASE = "task #";

static constexpr auto TRACES_BUFF_LEN = 0x800;  // 2K

namespace freertos_tracer = event_tracer::freertos;

int main()
{
    std::array<std::byte, TRACES_BUFF_LEN> traces_buff;

    auto get_steady_time = []() -> uint64_t {
        using namespace std::chrono;
        return duration_cast<microseconds>(steady_clock::now().time_since_epoch()).count();
    };

    auto consume_traces = [](freertos_tracer::EventRegistry &registry, freertos_tracer::data_done_cb_t done_cb) {
        for (const auto &event : registry) {
            std::printf(freertos_tracer::format(event).data());
        }
        done_cb();
    };

    freertos_tracer::SingleClient::emplace(
        freertos_tracer::Client::Settings{.buff = event_tracer::Span(traces_buff.data(), traces_buff.size()),
                                          .get_timestamp_cb = std::move(get_steady_time)},
        std::move(consume_traces));

    const auto task = [](void *) {
        std::random_device dev;
        std::mt19937 rng(dev());
        std::uniform_int_distribution<std::mt19937::result_type> sleep_dist(TASK_SLEEP_RANGE_MS.first,
                                                                            TASK_SLEEP_RANGE_MS.second);

        const auto task_sleep_ms = sleep_dist(rng);
        while (true) {
            auto *buff = pvPortMalloc(1000);
            std::cout << pcTaskGetName(nullptr) << std::endl;
            vPortFree(buff);
            vTaskDelay(task_sleep_ms / portTICK_PERIOD_MS);
        }
    };

    for (int i = 1; i <= TASK_NUM; ++i) {
        const auto task_name = std::string(TASK_NAME_BASE) + std::to_string(i);
        xTaskCreate(task, task_name.c_str(), MIN_TASK_STACK_SIZE, nullptr, MAX_TASK_PRIORITY, nullptr);
    }

    vTaskStartScheduler();
    return 0;
}

extern "C" void vLoggingPrintf(const char *pcFormatString, ...)
{
    va_list args;
    va_start(args, pcFormatString);
    vprintf(pcFormatString, args);
    printf("\n");
    va_end(args);
}

extern "C" void vApplicationMallocFailedHook(void)
{
    std::cout << "malloc failed" << std::endl;
    assert(false);
}

extern "C" void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName)
{
    std::cout << "stack overflow, task: \"" << std::string_view(pcTaskName) << "\"" << std::endl;
    assert(false);
}

extern "C" void vAssertCalled(const char *const pcFileName, unsigned long ulLine)
{
    std::cout << "assert at " << std::string_view(pcFileName) << ":" << ulLine << std::endl;
    assert(false);
}
