#pragma once

#include <assert.hpp>
#include <span.hpp>

namespace event_tracer
{

/// @brief Simple slab allocator with a free list for storage management.
/// @note Follows rapid allocation but slow deallocation strategy. Uses no additional memory except user-provided.
///       The minimum slab size cannot be smaller than a size of a pointer by design.
class SlabAllocator
{
public:
    SlabAllocator(Span<std::byte> storage, size_t slab_size) : m_begin(storage.data), m_slab_size(slab_size)
    {
        ET_ASSERT(storage);
        ET_ASSERT(storage.size_bytes() >= slab_size);
        ET_ASSERT(slab_size >= sizeof(Ptr));

        const size_t slabs = storage.size_bytes() / slab_size;
        m_end = m_begin + (slabs * slab_size);
        m_free_list = &m_begin;

        Ptr prev = m_begin;
        Ptr next = m_begin + slab_size;
        for (; next < m_end; next += slab_size) {
            *reinterpret_cast<Ptr*>(prev) = next;
            prev = next;
        }

        ET_ASSERT(prev + slab_size == m_end);
        *reinterpret_cast<Ptr*>(prev) = nullptr;
    }

    void* allocate()
    {
        if (!m_free_list) return nullptr;

        Ptr* next = reinterpret_cast<Ptr*>(*m_free_list);
        Ptr free = *m_free_list;
        m_free_list = next;

        return free;
    }

    void deallocate(void* slab)
    {
        if (slab < m_begin || slab >= m_end) return;

        Ptr* last = m_free_list;
        while (*last != nullptr) {
            last = reinterpret_cast<Ptr*>(*last);
        }

        *last = static_cast<Ptr>(slab);
        *reinterpret_cast<Ptr*>(last) = nullptr;
    }

    [[nodiscard]] size_t get_slab_size() const { return m_slab_size; }

private:
    using Ptr = std::byte*;

    Ptr m_begin;
    Ptr m_end;

    Ptr* m_free_list;

    const size_t m_slab_size;
};

}  // namespace event_tracer
