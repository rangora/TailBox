#pragma once

#include "Core.h"
#include <functional>

namespace tb
{
    enum class EventType
    {
        RenderInfoDisplay,
        CameraInfoDisplay
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

    struct RenderInfoDisplayEvent : public Event
    {
        RenderInfoDisplayEvent(int32 fps, float deltaTime) : _fps(fps), _deltaTime(deltaTime) {}

        float GetDeltaTime() { return _deltaTime; }
        int32 GetFps() { return _fps; }

        EVENT_TYPE(RenderInfoDisplay)

    private:
        int32 _fps = 0;
        float _deltaTime = 0.f;
    };

    struct CameraInfoDisplayEvent : public Event
    {
        CameraInfoDisplayEvent(XMFLOAT3 pos, XMFLOAT3 rot) : _pos(pos), _rot(rot) {}

        XMFLOAT3 GetPos() { return _pos; }
        XMFLOAT3 GetRot() { return _rot; }

        EVENT_TYPE(CameraInfoDisplay)

    private:
        XMFLOAT3 _pos;
        XMFLOAT3 _rot;
    };
} // namespace tb