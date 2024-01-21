#pragma once

#include <variant>

namespace event_tracer
{

struct Alternative
{
    static constexpr auto ignore = [](auto) {};
};

template <class... Ts>
struct Alternatives : Ts...
{
    using Ts::operator()...;
};

template <class... Ts>
Alternatives(Ts...) -> Alternatives<Ts...>;

}  // namespace event_tracer
