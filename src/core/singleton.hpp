#pragma once

#include <assert.hpp>

#include <optional>

namespace event_tracer::freertos
{

/// @brief Singleton helper. Support construction with params and accesing
/// @tparam TTarget Target type
template <typename TTarget>
class Singleton
{
public:
    template <typename... TArgs>
    static void emplace(TArgs&&... args)
    {
        m_instance.emplace(std::forward<TArgs>(args)...);
    }

    static TTarget& instance()
    {
        ET_ASSERT(m_instance.has_value());
        return *m_instance;
    }

private:
    inline static std::optional<TTarget> m_instance = std::nullopt;
};

}  // namespace event_tracer::freertos
