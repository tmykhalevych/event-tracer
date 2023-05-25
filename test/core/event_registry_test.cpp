#include <gtest/gtest.h>
#include <error_assert_impl.hpp>
#include <event_registry.hpp>

#include <iterator>

using TestEventDesc = event_tracer::EventDesc<>;
using TestEventRegistry = event_tracer::EventRegistry<TestEventDesc>;

namespace event_tracer
{

class EventRegistryTester
{
public:
    explicit EventRegistryTester(const TestEventRegistry& registry) : m_registry(registry) {}
    std::optional<uint64_t> get_first_ts() const { return m_registry.m_first_ts; }
    const TestEventDesc* get_begin() const { return m_registry.m_begin; }
    const TestEventDesc* get_next() const { return m_registry.m_next; }

private:
    const TestEventRegistry& m_registry;
};

} // namespace event_tracer

using event_tracer::EventRegistryTester;

TEST(EventRegistry, CreateWithNullptrOrZeroSize)
{
    {
        CLEAR_ERROR_ASSERT();
        TestEventRegistry invalid_registry(nullptr, 10);
        EXPECT_ASSERT();
    }
    {
        CLEAR_ERROR_ASSERT();
        TestEventDesc event_desc_arr[2];
        TestEventRegistry invalid_registry(event_desc_arr, 0);
        EXPECT_ASSERT();
    }
    {
        CLEAR_ERROR_ASSERT();
        TestEventRegistry invalid_registry(0);
        EXPECT_ASSERT();
    }
}

TEST(EventRegistry, Add)
{
    CLEAR_ERROR_ASSERT();
    TestEventRegistry registry(5);
    EventRegistryTester tester(registry);

    for (int i = 0; i < 3; ++i) {
        registry.add(TestEventDesc{});
    }

    EXPECT_EQ(3, std::distance(tester.get_begin(), tester.get_next()));
    EXPECT_FALSE(registry.empty());

    bool cb_called = false;
    registry.set_ready_cb([&cb_called, &registry](TestEventRegistry& ptr){
        cb_called = true;
        EXPECT_EQ(&registry, &ptr);
    });

    for (int i = 0; i < 2; ++i) {
        registry.add(TestEventDesc{});
    }
    EXPECT_TRUE(cb_called);

    registry.add(TestEventDesc{});
    EXPECT_ERROR();
    CLEAR_ERROR_ASSERT();

    registry.reset();
    registry.add(TestEventDesc{});

    EXPECT_NO_ERROR_OR_ASSERT();
}

TEST(EventRegistry, SoftReset)
{
    CLEAR_ERROR_ASSERT();
    TestEventRegistry registry(1);
    EventRegistryTester tester(registry);

    registry.add(TestEventDesc{.ts = 42});
    EXPECT_FALSE(registry.empty());

    registry.reset();
    EXPECT_TRUE(registry.empty());
    ASSERT_TRUE(tester.get_first_ts().has_value());
    EXPECT_EQ(42, tester.get_first_ts().value());

    EXPECT_NO_ERROR_OR_ASSERT();
}

TEST(EventRegistry, HardReset)
{
    CLEAR_ERROR_ASSERT();
    TestEventRegistry registry(1);
    EventRegistryTester tester(registry);

    registry.add(TestEventDesc{.ts = 42});
    EXPECT_FALSE(registry.empty());

    registry.reset(true);
    EXPECT_TRUE(registry.empty());
    EXPECT_FALSE(tester.get_first_ts().has_value());

    EXPECT_NO_ERROR_OR_ASSERT();
}

TEST(EventRegistry, Iterate)
{
    CLEAR_ERROR_ASSERT();
    TestEventRegistry registry(10);

    for (int i = 0; i < 10; ++i) {
        registry.add(TestEventDesc{});
    }

    int cnt = 0;
    for (const auto& desc : registry) ++cnt;
    EXPECT_EQ(10, cnt);

    EXPECT_NO_ERROR_OR_ASSERT();
}

TEST(EventRegistry, SetStartTimestamp)
{
    CLEAR_ERROR_ASSERT();
    TestEventRegistry registry(10);

    registry.set_start_timestamp(42);

    registry.add(TestEventDesc{.ts = 0});
    EXPECT_TRUE(registry.empty());

    registry.add(TestEventDesc{.ts = 50});
    EXPECT_FALSE(registry.empty());

    EXPECT_NO_ERROR_OR_ASSERT();
}
