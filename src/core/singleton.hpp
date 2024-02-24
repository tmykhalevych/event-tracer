#pragma once

#include <optional>

namespace event_tracer
{

/// @brief Singleton helper. Support construction with params and accesing
/// @tparam T Target type
template <typename T>
class Singleton
{
public:
    using Ptr = T*;

    template <typename... TArgs>
    static void emplace(TArgs&&... args)
    {
        if (m_instance) return;
        m_instance.emplace(std::forward<TArgs>(args)...);
    }

    static Ptr instance() { return m_instance.has_value() ? &m_instance.value() : nullptr; }
    static void reset() { m_instance.reset(); }

private:
    inline static std::optional<T> m_instance = std::nullopt;
};

}  // namespace event_tracer
