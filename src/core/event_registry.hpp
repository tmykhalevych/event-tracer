#pragma once

#include <assert.hpp>
#include <error.hpp>
#include <event.hpp>
#include <inplace_function.hpp>
#include <prohibit_copy_move.hpp>
#include <span.hpp>

#include <cstdint>
#include <optional>

namespace event_tracer
{

/// @brief Event registry collection
/// @tparam E Event descriptor concrete type
/// @warning Not MT-safe
template <typename E>
class EventRegistry : public ProhibitCopy
{
public:
    using event_t = E;
    using ready_cb_t = InplaceFunction<void(EventRegistry<event_t>&)>;

    explicit EventRegistry(Span<event_t> buff);
    ~EventRegistry();

    void add(event_t event);
    void reset(bool hard = false);

    const event_t* begin() const { return m_begin; }
    const event_t* end() const { return m_next; }

    [[nodiscard]] bool empty() const { return m_next == m_begin; }

    template <typename F>
    void set_ready_cb(F handler)
    {
        m_ready_cb = handler;
    }

    void set_start_timestamp(uint64_t ts) { m_first_ts = ts; }

private:
    size_t m_capacity;

    event_t* m_begin;
    event_t* m_next;

    std::optional<uint64_t> m_first_ts;
    ready_cb_t m_ready_cb;

    friend class EventRegistryTester;
};

template <typename ED>
EventRegistry<ED>::EventRegistry(Span<event_t> buff) : m_capacity(buff.size), m_begin(buff.data), m_next(m_begin)
{
    ET_ASSERT(buff);
}

template <typename ED>
EventRegistry<ED>::~EventRegistry()
{
    if (!empty() && m_ready_cb) m_ready_cb(*this);
}

template <typename ED>
void EventRegistry<ED>::add(event_t event)
{
    if (!m_first_ts) {
        m_first_ts = static_cast<uint64_t>(event.ts);
    }
    else if (*m_first_ts > event.ts) {
        // do not register events from the past, if we set start timestamp
        return;
    }

    const event_t* end = m_begin + m_capacity;
    if (m_next == end) {
        ET_ERROR("Buffer full, please reset before use");
        return;
    }

    event.ts -= *m_first_ts;
    *m_next = event;
    ++m_next;

    if (m_next == end && m_ready_cb) m_ready_cb(*this);
}

template <typename ED>
void EventRegistry<ED>::reset(bool hard)
{
    m_next = m_begin;
    if (hard) m_first_ts.reset();
}

}  // namespace event_tracer
