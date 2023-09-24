#pragma once

#include <variant>

namespace event_tracer
{

template <class... Ts>
struct Alternatives : Ts...
{
    using Ts::operator()...;
};

template <class... Ts>
Alternatives(Ts...) -> Alternatives<Ts...>;

}  // namespace event_tracer
