#include <FreeRTOS.h>
#include <task.h>

#include <chrono>
#include <iostream>
#include <random>
#include <string_view>
#include <utility>

static constexpr auto MAX_TASK_PRIORITY = configTIMER_TASK_PRIORITY - 2;
static const auto MIN_TASK_STACK_SIZE = configMINIMAL_STACK_SIZE;
static constexpr std::pair<int, int> TASK_SLEEP_RANGE_MS = {100, 500};
static constexpr auto TASK_NUM = 5;
static constexpr auto TASK_NAME_BASE = "task #";
static constexpr auto DATA_REGISTRY_QUEUE_SIZE = 2;
static constexpr auto TRACES_BUFF_LEN = 0x800;  // 2K
static uint8_t TRACES_BUFF[TRACES_BUFF_LEN];

int main()
{
    auto get_steady_time = []() -> uint64_t {
        using namespace std::chrono;
        return duration_cast<microseconds>(steady_clock::now().time_since_epoch()).count();
    };

    traces_init({TRACES_BUFF, TRACES_BUFF_LEN, get_steady_time, printf, DATA_REGISTRY_QUEUE_SIZE});

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
            vTaskDelay(task_sleep_ms);
        }
    };

    for (int i = 1; i <= TASK_NUM; ++i) {
        const auto task_name = std::string(TASK_NAME_BASE) + std::to_string(i);
        xTaskCreate(task, task_name.c_str(), MIN_TASK_STACK_SIZE, nullptr, MAX_TASK_PRIORITY, nullptr);
    }

    vTaskStartScheduler();
    return 0;
}

/// @brief Macro to stuck forever
#define STUCK      \
    while (true) { \
    }

extern "C" void vApplicationMallocFailedHook(void)
{
    std::cerr << "malloc failed" << std::endl;
    STUCK;
}

extern "C" void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName)
{
    std::cerr << "stack overflow, task: " << std::string_view(pcTaskName) << std::endl;
    STUCK;
}

extern "C" void vAssertCalled(const char *const pcFileName, unsigned long ulLine)
{
    std::cerr << "assert at " << std::string_view(pcFileName) << ":" << ulLine << std::endl;
    STUCK;
}
