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
    static std::optional<Message> create(std::string_view src, SlabAllocator& alloc)
    {
        ET_ASSERT(Capacity <= alloc.get_slab_size());

        char* storage = alloc.allocate();
        if (!storage) return std::nullopt;

        std::strncpy(storage, src.data(), Capacity);
        return Message{.m_data = storage};
    }

    static void destroy(Message msg, SlabAllocator& alloc) { alloc.deallocate(msg.m_data); }

    [[nodiscard]] char* c_str() const { return m_data; }

private:
    Message() = default;

    char* m_data;
};

}  // namespace event_tracer::freertos
