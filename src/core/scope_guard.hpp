#pragma once

#include <prohibit_copy_move.hpp>

#include <functional>
#include <optional>

namespace event_tracer
{

/// @brief Basic scope guard. Invokes final action when out of scope. Doesn't invoke when canceled.
/// @tparam TFinalAction Invocable final action type
template <typename TFinalAction>
class ScopeGuard : public ProhibitCopyMove
{
public:
    ScopeGuard() = delete;

    explicit ScopeGuard(TFinalAction action) : m_action(std::move(action)) {}
    ~ScopeGuard()
    {
        if (m_action) std::invoke(*m_action);
    }

    void cancel() { m_action.reset(); }

private:
    std::optional<TFinalAction> m_action;
};

}  // namespace event_tracer
