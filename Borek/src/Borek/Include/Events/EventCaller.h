// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <functional>

#include "Event.h"

namespace Borek {

class EventCaller {
        template <class T>
        using EventFn = std::function<bool(T&)>;
public:
        EventCaller(Event& e) : m_Event(e) {}

        template <class T>
        bool TryCall(EventFn<T> fn) const {
                if (m_Event.GetEventType() == T::ClassEventType())
                        m_Event.m_Handled = fn(reinterpret_cast<T&>(m_Event));
                
                return m_Event.m_Handled;
        }

private:
        Event& m_Event;
};

}  // namespace Borek
