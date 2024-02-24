#pragma once

namespace event_tracer
{

/// @brief Helper class to prohibit derived class copy operations
class ProhibitCopy
{
public:
    ProhibitCopy(const ProhibitCopy&) = delete;
    ProhibitCopy& operator=(const ProhibitCopy&) = delete;

protected:
    ProhibitCopy() = default;
};

/// @brief Helper class to prohibit derived class move operations
class ProhibitMove
{
public:
    ProhibitMove(ProhibitMove&&) = delete;
    ProhibitMove& operator=(ProhibitMove&&) = delete;

protected:
    ProhibitMove() = default;
};

/// @brief Helper class to prohibit derived class copy and move operations
class ProhibitCopyMove : public ProhibitCopy, public ProhibitMove
{
protected:
    ProhibitCopyMove() = default;
};

}  // namespace event_tracer
