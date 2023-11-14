#pragma once

#include <assert.hpp>
#include <prohibit_copy_move.hpp>

#include <array>
#include <cstdint>
#include <functional>
#include <type_traits>
#include <utility>

namespace event_tracer
{

namespace details
{

/// @brief Simple wrapper for types
template <typename T>
struct Wrapper
{
    using type = T;
};

/// @brief The implementation of virtual table for callable object stored in InplaceFunction
template <typename TRet, typename... TArgs>
struct VTable : public ProhibitCopyMove
{
    using storage_ptr_t = void *;

    using invoke_ptr_t = TRet (*)(storage_ptr_t, TArgs &&...);
    using process_ptr_t = void (*)(storage_ptr_t, storage_ptr_t);
    using destruct_ptr_t = void (*)(storage_ptr_t);

    constexpr VTable()
        : invoke([](storage_ptr_t, TArgs &&...) {
            ET_ASSERT(!"Invoking empty object");
            return TRet{};
        })
        , copy([](storage_ptr_t, storage_ptr_t) {})
        , move([](storage_ptr_t, storage_ptr_t) {})
        , destruct([](storage_ptr_t) {})
    {}

    template <typename Callable>
    explicit constexpr VTable(Wrapper<Callable> /* ignore */)
        : invoke([](storage_ptr_t storage_ptr, TArgs &&...args) {
            return std::invoke(*static_cast<Callable *>(storage_ptr), args...);
        })
        , copy([](storage_ptr_t dst_ptr, storage_ptr_t src_ptr) {
            new (dst_ptr) Callable(*static_cast<Callable *>(src_ptr));
        })
        , move([](storage_ptr_t dst_ptr, storage_ptr_t src_ptr) {
            new (dst_ptr) Callable(std::move(*static_cast<Callable *>(src_ptr)));
            static_cast<Callable *>(src_ptr)->~Callable();
        })
        , destruct([](storage_ptr_t src_ptr) { static_cast<Callable *>(src_ptr)->~Callable(); })
    {}

    ~VTable() = default;

    const invoke_ptr_t invoke;
    const process_ptr_t copy;
    const process_ptr_t move;
    const destruct_ptr_t destruct;
};

template <typename TRet, typename... TArgs>
inline constexpr VTable<TRet, TArgs...> empty_vtable{};

template <size_t DstCapacity, size_t SrcCapacity>
struct is_sufficient_dst : std::true_type
{
    static_assert(DstCapacity >= SrcCapacity, "Destination storage capacity is insufficient");
};

}  // namespace details

/// @brief Default callable object size for InplaceFunction
static constexpr auto DEFAULT_CALLABLE_SIZE = 4 * sizeof(void *);

/// @brief Simple callable type wrapper. Designed to be used instead of std::function. Does zero heap allocations.
/// @tparam F Callable object invocation signature, e.g. void(int)
/// @tparam Capacity Max size of internal buffer for storing callable object. Default is (4 * pointer size)
template <typename F, size_t Capacity = DEFAULT_CALLABLE_SIZE>
class InplaceFunction;

namespace details
{

template <typename>
struct is_inplace_function : std::false_type
{
};
template <typename F, size_t Capacity>
struct is_inplace_function<InplaceFunction<F, Capacity>> : std::true_type
{
};

}  // namespace details

template <typename TRet, typename... TArgs, size_t Capacity>
class InplaceFunction<TRet(TArgs...), Capacity>
{
public:
    InplaceFunction() : m_vtable_ptr(std::addressof(details::empty_vtable<TRet, TArgs...>)) {}
    InplaceFunction(nullptr_t) : InplaceFunction() {}

    template <size_t OtherCapacity>
    InplaceFunction(const InplaceFunction<TRet(TArgs...), OtherCapacity> &other)
        : InplaceFunction(other.m_vtable_ptr, other.m_vtable_ptr->copy_ptr, std::addressof(other.m_storage))
    {
        static_assert(details::is_sufficient_dst<Capacity, OtherCapacity>::value, "Insufficient capacity");
    }

    template <size_t OtherCapacity>
    InplaceFunction(InplaceFunction<TRet(TArgs...), OtherCapacity> &&other)
        : InplaceFunction(other.m_vtable_ptr, other.m_vtable_ptr->move, std::addressof(other.m_storage))
    {
        static_assert(details::is_sufficient_dst<Capacity, OtherCapacity>::value, "Insufficient capacity");
        other.m_vtable_ptr = std::addressof(details::empty_vtable<TRet, TArgs...>);
    }

    InplaceFunction(const InplaceFunction &other) : m_vtable_ptr(other.m_vtable_ptr)
    {
        m_vtable_ptr->copy(std::addressof(m_storage), std::addressof(other.m_storage));
    }

    InplaceFunction(InplaceFunction &&other)
        : m_vtable_ptr(std::exchange(other.m_vtable_ptr, std::addressof(details::empty_vtable<TRet, TArgs...>)))
    {
        m_vtable_ptr->move(std::addressof(m_storage), std::addressof(other.m_storage));
    }

    InplaceFunction &operator=(nullptr_t)
    {
        m_vtable_ptr->destruct(std::addressof(m_storage));
        m_vtable_ptr = std::addressof(details::empty_vtable<TRet, TArgs...>);
        return *this;
    }

    InplaceFunction &operator=(InplaceFunction other)
    {
        m_vtable_ptr->destruct(std::addressof(m_storage));

        m_vtable_ptr = std::exchange(other.m_vtable_ptr, std::addressof(details::empty_vtable<TRet, TArgs...>));
        m_vtable_ptr->move(std::addressof(m_storage), std::addressof(other.m_storage));
        return *this;
    }

    template <typename F, typename Callable = std::decay_t<F>,
              typename = std::enable_if_t<!details::is_inplace_function<Callable>::value &&
                                          std::is_invocable_r<TRet, Callable &, TArgs...>::value>>
    InplaceFunction(F f)
    {
        static_assert(std::is_copy_constructible<Callable>::value, "Cannot be constructed from non-copyable type");
        static_assert(sizeof(Callable) <= Capacity, "Insufficient capacity");

        static const vtable_t vtable(details::Wrapper<Callable>{});
        m_vtable_ptr = std::addressof(vtable);

        new (std::addressof(m_storage)) Callable(std::forward<F>(f));
    }

    ~InplaceFunction() { m_vtable_ptr->destruct(std::addressof(m_storage)); }

    TRet operator()(TArgs... args) const
    {
        return m_vtable_ptr->invoke(std::addressof(m_storage), std::forward<TArgs>(args)...);
    }

    [[nodiscard]] constexpr bool operator==(nullptr_t) const { return !operator bool(); }
    [[nodiscard]] constexpr bool operator!=(nullptr_t) const { return operator bool(); }

    [[nodiscard]] constexpr operator bool() const
    {
        return m_vtable_ptr != std::addressof(details::empty_vtable<TRet, TArgs...>);
    }

private:
    using vtable_t = details::VTable<TRet, TArgs...>;
    using process_ptr_t = typename vtable_t::process_ptr_t;
    using storage_ptr_t = typename vtable_t::storage_ptr_t;

    InplaceFunction(const vtable_t *vtable_ptr, process_ptr_t process_ptr, storage_ptr_t storage_ptr)
        : m_vtable_ptr{vtable_ptr}
    {
        process_ptr(std::addressof(m_storage), storage_ptr);
    }

    const vtable_t *m_vtable_ptr;
    mutable std::array<uint8_t, Capacity> m_storage;
};

}  // namespace event_tracer
