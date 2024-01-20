#pragma once

#include <assert.hpp>

#include <utility>

namespace event_tracer
{

/// @brief Basic buffer slice. Keeps pointer together with size
/// @tparam T Buffer content type
template <typename T>
struct Slice
{
    Slice() = default;
    Slice(T* data, size_t size) : m_data(data), m_size(size) {}

    ~Slice() = default;

    /// @brief Cut the chunk from a beginning of a slice
    /// @tparam C Cut chunk content type. Same as for original slice, by default.
    /// @param size A size of a chunk to cut
    /// @return A pair of [cut, rest] parts of a slice
    template <typename C = T>
    std::pair<Slice<C>, Slice> cut(size_t size)
    {
        ET_ASSERT(size * sizeof(C) + sizeof(T) <= size_bytes());

        Slice<C> first_part(reinterpret_cast<C*>(m_data), size);

        std::byte* raw_data = reinterpret_cast<std::byte*>(m_data);
        const size_t cut_size_bytes = size * sizeof(C);
        T* new_data = reinterpret_cast<T*>(raw_data + cut_size_bytes);
        const size_t new_size = (size_bytes() - cut_size_bytes) / sizeof(T);

        Slice second_part(new_data, new_size);

        return std::make_pair(first_part, second_part);
    }

    /// @brief Cut the slice into two equal pieces
    /// @return A pair of equal parts of a slice
    std::pair<Slice, Slice> bifurcate() { return cut(m_size / 2); }

    /// @brief Transforms slice into slice of new content type
    /// @tparam N New content type
    /// @return The slice of new content type
    template <typename N>
    Slice<N> transform()
    {
        ET_ASSERT(sizeof(N) <= size_bytes());

        N* new_data = reinterpret_cast<N*>(m_data);
        const size_t new_size = size_bytes() / sizeof(N);

        return Slice<N>(new_data, new_size);
    }

    [[nodiscard]] T* const data() const { return m_data; }
    [[nodiscard]] size_t size() const { return m_size; }
    [[nodiscard]] size_t size_bytes() const { return sizeof(T) * m_size; }

    [[nodiscard]] operator bool() const { return (m_data != nullptr) && (m_size > 0); }

    const T* begin() const { return m_data; }
    const T* end() const { return m_data + m_size; }

private:
    T* m_data = nullptr;
    size_t m_size = 0;
};

}  // namespace event_tracer
