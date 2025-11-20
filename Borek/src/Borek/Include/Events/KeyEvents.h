// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Events/Event.h"
#include "Include/Base/Input.h"

namespace Borek {

class KeyEvent : public Event {
_BASE_EVENT_HEADER(KeyEvent)
public:
        KeyEvent(KeyCode keycode, bool pressed, bool repeated = false) :
                m_KeyCode(keycode), m_Pressed(pressed), m_Repeated(repeated) {}

        inline KeyCode GetKeycode() const { return m_KeyCode; }
        inline bool IsRepeated() const { return m_Repeated; }
        inline bool IsPressed() const { return m_Pressed; }

protected:
        KeyCode m_KeyCode;
        bool m_Pressed;
        bool m_Repeated;
};

class KeyTypedEvent : public Event {
_BASE_EVENT_HEADER(KeyTypedEvent)
public:
        KeyTypedEvent(KeyCode keycode) : m_KeyCode(keycode) {}

        inline KeyCode GetKeycode() const { return m_KeyCode; }

protected:
        KeyCode m_KeyCode;
};

}  // namespace borek
