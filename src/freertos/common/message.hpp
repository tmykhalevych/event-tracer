#pragma once

#include <assert.hpp>
#include <slab_allocator.hpp>

#include <cstring>
#include <limits>
#include <optional>
#include <string_view>

namespace event_tracer::freertos
{

/// @brief Allows user to create, destroy and access C-string using SlabAllocator.
///        Stores index of a slab, where message is located.
/// @tparam C Expected maxim message length (including '\0')
/// @warning Message is not owning the buffer. This is made to keep Message size as small as possible and not store
///          a pointer to allocator and/or even C-string. Please DO NOT FORGET to destroy the message.
///          RAII is not followed by design.
template <unsigned C>
class Message
{
public:
    static std::optional<Message> create(std::string_view src, SlabAllocator& msg_pool)
    {
        ET_ASSERT(C <= msg_pool.slab_size());
        ET_ASSERT(msg_pool.capacity() <= std::numeric_limits<index_t>::max());

        SlabAllocator::Ptr free = msg_pool.allocate();
        if (!free) {
            return std::nullopt;
        }

        std::strncpy(reinterpret_cast<char*>(free), src.data(), C);

        return Message(std::distance(msg_pool.data(), free) / msg_pool.slab_size());
    }

    static void destroy(const Message& msg, SlabAllocator& msg_pool)
    {
        ET_ASSERT(msg_pool.capacity() >= msg.m_data_index);

        msg_pool.deallocate(msg_pool.data() + msg.m_data_index * msg_pool.slab_size());
    }

    [[nodiscard]] char* get(SlabAllocator& msg_pool) const
    {
        ET_ASSERT(msg_pool.capacity() >= m_data_index);

        return reinterpret_cast<char*>(msg_pool.data() + m_data_index * msg_pool.slab_size());
    }

private:
    using index_t = uint8_t;

    explicit Message(index_t index) : m_data_index(index) {}

    index_t m_data_index;
};

}  // namespace event_tracer::freertos
