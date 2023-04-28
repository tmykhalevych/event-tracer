#include <FreeRTOS.h>
#include <task.h>

#include <iostream>
#include <string_view>
#include <random>
#include <utility>

/// @brief Infinite loop, used to never exit tasks
#define NEVER_RETURN while (true) {}

static constexpr auto MAX_TASK_PRIORITY = configTIMER_TASK_PRIORITY - 1;
static constexpr auto MIN_TASK_STACK_SIZE = configMINIMAL_STACK_SIZE;
static constexpr std::pair<int, int> TASK_SLEEP_RANGE_MS = {100, 500};
static constexpr auto TASK_NUM = 5;
static constexpr auto TASK_NAME_BASE = "task #";

int main()
{
    const auto task = [](void*) {
        std::random_device dev;
        std::mt19937 rng(dev());
        std::uniform_int_distribution<std::mt19937::result_type> sleep_dist(TASK_SLEEP_RANGE_MS.first,
                                                                            TASK_SLEEP_RANGE_MS.second);

        const auto task_sleep_ms = sleep_dist(rng);
        while (true) {
            std::cout << pcTaskGetName(nullptr) << std::endl;
            vTaskDelay(task_sleep_ms);
        }

        NEVER_RETURN
    };

    for (int i = 1; i <= TASK_NUM; ++i) {
        const auto task_name = std::string(TASK_NAME_BASE) + std::to_string(i);
        xTaskCreate(task, task_name.c_str(), MIN_TASK_STACK_SIZE, nullptr, MAX_TASK_PRIORITY, nullptr);
    }

    vTaskStartScheduler();
    return 0;
}

extern "C" void vApplicationMallocFailedHook(void)
{
    std::cerr << "malloc failed" << std::endl;
}

extern "C" void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName)
{
    std::cerr << "stack overflow, task: " << std::string_view(pcTaskName) << std::endl;
}

extern "C" void vAssertCalled(const char *const pcFileName, unsigned long ulLine)
{
    std::cerr << "assert at " << std::string_view(pcFileName) << ":" << ulLine << std::endl;
}
