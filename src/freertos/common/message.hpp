#pragma once

#include <assert.hpp>
#include <slab_allocator.hpp>

#include <cstring>
#include <optional>
#include <string_view>

namespace event_tracer::freertos
{

/// @brief Simple wrapper on C-string. Allows user to create/destroy C-string using SlabAllocator.
/// @tparam Capacity Expected maxim message length (including '\0')
/// @warning Message is not owning the buffer. This is made to keep Message size as small as possible and not store
///          a pointer to allocator. Please DO NOT FORGET to destroy the message. RAII is not followed by design.
template <size_t Capacity>
class Message
{
public:
    static std::optional<Message> create(std::string_view src, SlabAllocator& msg_pool)
    {
        ET_ASSERT(Capacity <= msg_pool.get_slab_size());

        char* dst = reinterpret_cast<char*>(msg_pool.allocate());
        if (!dst) return std::nullopt;

        std::strncpy(dst, src.data(), Capacity);
        return Message(dst);
    }

    static void destroy(const Message& msg, SlabAllocator& msg_pool)
    {
        auto* data = reinterpret_cast<SlabAllocator::Ptr>(msg.m_data);
        msg_pool.deallocate(data);
    }

    [[nodiscard]] char* c_str() const { return m_data; }

private:
    explicit Message(char* underlying) : m_data(underlying) {}

    char* m_data;
};

}  // namespace event_tracer::freertos
