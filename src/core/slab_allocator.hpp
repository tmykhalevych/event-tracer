#pragma once

#include <assert.hpp>
#include <error.hpp>
#include <prohibit_copy_move.hpp>
#include <slice.hpp>

namespace event_tracer
{

/// @brief Simple slab allocator with a free list for storage management.
/// @note Follows rapid allocation but slow deallocation strategy. Uses no additional memory except user-provided.
///       The minimum slab size cannot be smaller than a size of a pointer by design.
class SlabAllocator : public ProhibitCopy
{
public:
    using Ptr = std::byte*;

    SlabAllocator(Slice<std::byte> storage, size_t slab_size) : m_begin(storage.data()), m_slab_size(slab_size)
    {
        ET_ASSERT(storage);
        ET_ASSERT(storage.size_bytes() >= slab_size);
        ET_ASSERT(slab_size >= sizeof(Ptr));

        const size_t slabs = storage.size_bytes() / slab_size;
        m_end = m_begin + (slabs * slab_size);
        m_free_list = m_begin;

        Ptr prev = m_begin;
        Ptr next = m_begin + slab_size;
        for (; next < m_end; next += slab_size) {
            *reinterpret_cast<Ptr*>(prev) = next;
            prev = next;
        }

        ET_ASSERT(prev + slab_size == m_end);
        *reinterpret_cast<Ptr*>(prev) = nullptr;
    }

    Ptr allocate()
    {
        ET_ASSERT(m_begin);

        if (!m_free_list) {
            ET_ERROR("Cannot allocate on pool");
            return nullptr;
        }

        Ptr next = *reinterpret_cast<Ptr*>(m_free_list);
        Ptr free = m_free_list;
        m_free_list = next;

        return free;
    }

    void deallocate(Ptr free)
    {
        ET_ASSERT(m_begin);
        ET_ASSERT(free >= m_begin || free <= m_end);

        if (!m_free_list) {
            m_free_list = free;
            *reinterpret_cast<Ptr*>(free) = nullptr;
            return;
        }

        Ptr prev = nullptr;
        Ptr last = m_free_list;
        while (last) {
            prev = last;
            last = *reinterpret_cast<Ptr*>(last);
        }

        *reinterpret_cast<Ptr*>(prev) = free;
        *reinterpret_cast<Ptr*>(free) = nullptr;
    }

    [[nodiscard]] size_t get_slab_size() const { return m_slab_size; }

private:
    Ptr m_begin = nullptr;
    Ptr m_end = nullptr;
    Ptr m_free_list = nullptr;

    size_t m_slab_size = 0;
};

}  // namespace event_tracer
