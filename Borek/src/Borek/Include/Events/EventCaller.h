// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <concepts>
#include <functional>

#include "Include/Events/Event.h"
#include "Include/Events/CustomEvent.h"

namespace Borek {

class EventCaller {
        template <class T>
        using EventFn = std::function<bool(T&)>;
public:
        EventCaller(Event& e) : m_Event(e) {}

        template <class T>
        requires(std::derived_from<T, Event> && !std::derived_from<T, CustomEvent<T>>)
        bool TryCall(EventFn<T> fn) const {
                if (m_Event.m_Handled) return true;

                if (m_Event.GetEventType() == T::ClassEventType())
                        m_Event.m_Handled = fn(reinterpret_cast<T&>(m_Event));
                
                return m_Event.m_Handled;
        }

        template <class T>
        requires(std::derived_from<T, CustomEvent<T>>)
        bool TryCall(EventFn<T> fn) const {
                if (m_Event.m_Handled) return true;
                if (!(m_Event.GetEventType() == EventType::kCustomEvent))
                        return false;

                T& casted_event = reinterpret_cast<T&>(m_Event);
                if (casted_event.GetEventId() == T::GetCustomEventId())
                        m_Event.m_Handled = fn(casted_event);
                
                return m_Event.m_Handled;
        }

private:
        Event& m_Event;
};

}  // namespace Borek
