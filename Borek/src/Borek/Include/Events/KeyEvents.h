// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <format>
#include <magic_enum/magic_enum.hpp>

#include "./Event.h"
#include "Include/Input.h"

namespace Borek {

class KeyEvent : public Event {
public:
        _EVENT_CLASS_CATEGORY(EventCategory::kInput | EventCategory::kKeyboard);

        inline KeyCode GetKeyCode() const { return m_KeyCode; }

protected:
        KeyEvent(KeyCode keycode) : m_KeyCode(keycode) {}
        KeyCode m_KeyCode;
};

class KeyPressedEvent : public KeyEvent {
public:
        _EVENT_CLASS_TYPE(KeyPressed);

        KeyPressedEvent(KeyCode keycode, bool repeated = false) :
                KeyEvent(keycode), m_Repeated(repeated) {}
        inline bool GetRepeated() const { return m_Repeated; }
        operator std::string() const override {
                return std::format("Event {}: (KeyCode: {} Repeated: {})",
                                   GetName(),
                                   magic_enum::enum_name(m_KeyCode),
                                   m_Repeated);
        }

protected:
        bool m_Repeated;
};

class KeyReleasedEvent : public KeyEvent {
public:
        _EVENT_CLASS_TYPE(KeyReleased);

        KeyReleasedEvent(KeyCode keycode) : KeyEvent(keycode) {}
        operator std::string() const override {
                return std::format("Event {}: (KeyCode: {})",
                                   GetName(),
                                   magic_enum::enum_name(m_KeyCode));
        }
};

class KeyTypedEvent : public KeyEvent {
public:
        _EVENT_CLASS_TYPE(KeyTyped);

        KeyTypedEvent(KeyCode keycode) : KeyEvent(keycode) {}
        operator std::string() const override {
                return std::format("Event {}: (KeyCode: {})",
                                   GetName(),
                                   magic_enum::enum_name(m_KeyCode));
        }
};

}  // namespace borek
