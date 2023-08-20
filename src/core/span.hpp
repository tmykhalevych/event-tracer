#pragma once

#include <assert.hpp>

namespace event_tracer
{

/// @brief Basic span. Represents buffer, keeps pointer together with size
/// @tparam T Buffer content type
template <typename T>
struct Span
{
    Span() = default;
    Span(T *data, size_t size) : data(data), size(size) {}

    ~Span() = default;

    [[nodiscard]] operator bool() const { return (data != nullptr) && (size > 0); }

    T *data = nullptr;
    size_t size = 0;
};

}  // namespace event_tracer
