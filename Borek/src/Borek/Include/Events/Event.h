// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include <string>
#include <ostream>

#pragma once

#define _EVENT_CLASS_TYPE(_event_type)                        \
        static const EventType ClassEventType()               \
        {                                                     \
                return EventType::k##_event_type;             \
        }                                                     \
        virtual const EventType GetEventType() const override \
        {                                                     \
                return EventType::k##_event_type;             \
        }                                                     \
        virtual const char *GetName() const override          \
        {                                                     \
                return #_event_type;                          \
        }

#define _EVENT_CLASS_CATEGORY(categories)                     \
        virtual const int GetEventCategories() const override \
        {                                                     \
                return categories;                            \
        }

namespace Borek
{

enum class EventType {
        kNone = 0,

        // Window events
        kWindowClose,
        kWindowResize,
        kWindowFocus,
        kWindowLostFocus,
        kWindowMoved,

        // Application events
        kTick,
        kUpdate,
        kRender,

        // KeyPress events
        kKeyPressed,
        kKeyReleased,
        kKeyTyped,

        // MouseEvents
        kMouseButtonPressed,
        kMouseButtonReleased,
        kMouseMoved,
        kMouseScrolled,

        kCustomEvent,
};

enum EventCategory {
        kNone = 0,
        kWindow = 1,
        kInput = 2,
        kKeyboard = 4,
        kMouse = 8,
        kMouseButton = 16,
        kApplication = 32,
        kCustom = 64,
};


class Event {
        friend class EventCaller;

public:
        virtual ~Event() = default;
        inline bool GetHandled() { return m_Handled; }
        inline void Handle() { m_Handled = true; }

        virtual const EventType GetEventType() const = 0;
        virtual const int GetEventCategories() const = 0;
        virtual const char *GetName() const = 0;
        virtual operator std::string() const
        {
                return GetName();
        };
        inline bool IncludesCategory(EventCategory category) const
        {
                return GetEventCategories() & category;
        }
        friend std::ostream& operator <<(std::ostream& os, const Borek::Event& ev)
        {
                return os << std::string(ev);
        }

protected:
        bool m_Handled = false;
};

} // namespace Borek
