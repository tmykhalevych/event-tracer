#pragma once

#include <event_desc.hpp>
#include <assert.hpp>
#include <error.hpp>

#include <cstdint>
#include <optional>
#include <functional>

namespace event_tracer
{

/// @brief Event registry collection
/// @tparam ED Event descriptor concrete type
/// @warning Not MT-safe
template<typename ED>
class EventRegistry
{
    static_assert(sizeof(ED) == 8, "EventDesk should be packed into 8 bytes");

public:
    using event_desc_t = ED;
    
    EventRegistry(event_desc_t* buff, size_t capacity);
    explicit EventRegistry(size_t capacity);

    ~EventRegistry();

    void add(event_desc_t event);
    void reset(bool hard = false);

    const event_desc_t* begin() const { return m_begin; }
    const event_desc_t* end() const { return m_next; }

    [[nodiscard]] bool empty() const { return m_next == m_begin; }

    template<typename F>
    void set_ready_cb(F handler) { m_ready_cb = handler; }
    void set_start_timestamp(uint64_t ts) { m_first_ts = ts; }

private:
    const size_t m_capacity;
    const bool m_heap_allocated;

    event_desc_t* m_begin;
    event_desc_t* m_next;

    std::optional<uint64_t> m_first_ts;
    std::function<void(EventRegistry<event_desc_t>&)> m_ready_cb;

    friend class EventRegistryTester;
};

template<typename ED>
EventRegistry<ED>::EventRegistry(event_desc_t* buff, size_t capacity)
    : m_capacity(capacity)
    , m_heap_allocated(false)
    , m_begin(buff)
    , m_next(m_begin)
{
    ASSERT(buff);
    ASSERT(capacity > 0);
}

template<typename ED>
EventRegistry<ED>::EventRegistry(size_t capacity)
    : m_capacity(capacity)
    , m_heap_allocated(true)
    , m_begin(new event_desc_t[capacity])
    , m_next(m_begin)
{
    ASSERT(capacity > 0);
}

template<typename ED>
EventRegistry<ED>::~EventRegistry()
{
    if (!empty() && m_ready_cb) m_ready_cb(*this);
    if (m_heap_allocated) delete [] m_begin;
}

template<typename ED>
void EventRegistry<ED>::add(event_desc_t event)
{
    if (!m_first_ts) {
        m_first_ts = static_cast<uint64_t>(event.ts);
    }
    else if (*m_first_ts > event.ts) {
        // do not register events from the past, if we set start timestamp
        return;
    }

    const event_desc_t* end = m_begin + m_capacity;
    if (m_next == end) {
        ERROR("Buffer full, please reset before use");
        return;
    }

    event.ts -= *m_first_ts;
    *m_next = event;
    ++m_next;

    if (m_next == end && m_ready_cb) m_ready_cb(*this);
}

template<typename ED>
void EventRegistry<ED>::reset(bool hard)
{
    m_next = m_begin;
    if (hard) m_first_ts.reset();
}

} // namespace event_tracer
