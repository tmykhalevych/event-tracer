#pragma once

#include <assert.hpp>

#include <array>
#include <cstdint>
#include <functional>
#include <memory>

namespace event_tracer
{

/// @brief Default callable object size for StaticFunction
static constexpr auto DEFAULT_CALLABLE_SIZE = 4 * sizeof(void *);

/// @brief Simple callable type wrapper. Designed to be used instead of std::function. Does zero heap allocations.
/// @tparam F Callable object invocation signature, e.g. void(int)
/// @tparam StorageSize Max size of internal buffer for storing callable object. Default is (4 * pointer size)
template <typename F, size_t StorageSize = DEFAULT_CALLABLE_SIZE>
class StaticFunction;

template <typename TRet, typename... TArgs, size_t StorageSize>
class StaticFunction<TRet(TArgs...), StorageSize>
{
public:
    StaticFunction() : m_callable(nullptr, CallableI::destruct) {}
    StaticFunction(nullptr_t) : StaticFunction() {}

    StaticFunction(StaticFunction &other) : StaticFunction() { *this = other; }
    StaticFunction(StaticFunction &&other) : StaticFunction() { *this = std::move(other); }

    template <typename F>
    StaticFunction(F f) : m_callable(new (m_storage.data()) Callable<F>(std::move(f)), CallableI::destruct)
    {
        static_assert(sizeof(F) <= StorageSize);
    }

    StaticFunction &operator=(StaticFunction &other)
    {
        std::copy(other.m_storage.begin(), other.m_storage.end(), m_storage.begin());
        m_callable.reset(reinterpret_cast<CallableI *>(m_storage.data()));
        return *this;
    }

    StaticFunction &operator=(StaticFunction &&other)
    {
        *this = other;
        other.m_callable.release();
        return *this;
    }

    ~StaticFunction() = default;

    TRet operator()(TArgs &&...args) { return call(std::forward<TArgs>(args)...); }
    TRet operator()(TArgs &&...args) const { return call(std::forward<TArgs>(args)...); }

    [[nodiscard]] operator bool() const { return m_callable != nullptr; }

private:
    struct CallableI
    {
        using ptr_t = std::unique_ptr<CallableI, void (*)(CallableI *)>;

        virtual TRet call(TArgs &&...) = 0;
        virtual ~CallableI() = default;

        static void destruct(CallableI *p)
        {
            if (p) p->~CallableI();
        }
    };

    template <typename F>
    struct Callable : public CallableI
    {
        Callable(F &&f) : m_f(std::move(f)) {}
        TRet call(TArgs &&...args) override { return std::invoke(m_f, std::forward<TArgs>(args)...); }

        F m_f;
    };

    TRet call(TArgs &&...args) const
    {
        ET_ASSERT(m_callable);
        return m_callable->call(std::forward<TArgs>(args)...);
    }

    typename CallableI::ptr_t m_callable;
    std::array<uint8_t, StorageSize> m_storage;
};

}  // namespace event_tracer
