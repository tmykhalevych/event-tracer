#pragma once

#include <optional>

namespace event_tracer::freertos
{

/// @brief Singleton helper. Support construction with params and accesing
/// @tparam TTarget Target type
template <typename TTarget>
class Singleton
{
public:
    using Ptr = TTarget*;

    template <typename... TArgs>
    static void emplace(TArgs&&... args)
    {
        m_instance.emplace(std::forward<TArgs>(args)...);
    }

    static Ptr instance() { return m_instance.has_value() ? &m_instance.value() : nullptr; }
    static void reset() { m_instance.reset(); }

private:
    inline static std::optional<TTarget> m_instance = std::nullopt;
};

}  // namespace event_tracer::freertos
