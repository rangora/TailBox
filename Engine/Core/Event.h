#pragma once

#include "Core.h"
#include <functional>

namespace tb
{
    enum class EventType
    {
    };

    #define EVENT_TYPE(type) static EventType GetStaticType() { return EventType::type; } \
    EventType GetEventType() const final { return GetStaticType(); }

    struct Event
    {
        bool _isHandled = false;

        virtual ~Event() {}
        virtual EventType GetEventType() const = 0;
    };

    class EventDispatcher
    {
        template<typename T>
        using EventFn = std::function<bool(T&)>;

    public:
        EventDispatcher(Event& e) : _e(e) {}

        template <typename T>
        bool Dispatch(EventFn<T> Fn)
        {
            if (_e._isHandled == false && _e.GetEventType() == T::GetStaticType())
            {
                _e._isHandled = Fn(static_cast<T&>(_e));
                return true;
            }

            return false;
        }

    private:
        Event & _e;
    };
} // namespace tb