#pragma once

namespace event_tracer
{

/// @brief Helper class to prohibit derived class copy operations
class ProhibitCopy
{
public:
    ProhibitCopy() = default;
    ProhibitCopy(const ProhibitCopy&) = delete;
    ProhibitCopy& operator=(const ProhibitCopy&) = delete;
};

/// @brief Helper class to prohibit derived class move operations
class ProhibitMove
{
public:
    ProhibitMove() = default;
    ProhibitMove(ProhibitMove&&) = delete;
    ProhibitMove& operator=(ProhibitMove&&) = delete;
};

/// @brief Helper class to prohibit derived class copy and move operations
class ProhibitCopyMove : public ProhibitCopy, public ProhibitMove
{
};

}  // namespace event_tracer
