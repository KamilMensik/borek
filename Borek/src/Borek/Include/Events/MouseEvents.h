// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "glm/ext/vector_float2.hpp"
#include <format>

#include <iostream>
#include <magic_enum/magic_enum.hpp>

#include "Include/Events/Event.h"
#include "Include/Base/Input.h"

namespace Borek {

class MouseMovedEvent : public Event {
public:
        _EVENT_CLASS_CATEGORY(kMouse);
        _EVENT_CLASS_TYPE(MouseMoved);

        MouseMovedEvent(float x, float y) : m_X(x), m_Y(y) {}

        operator std::string() const override
        {
                return std::format("Event {}: (X: {} Y: {})",
                                   GetName(), m_X, m_Y);
        }

        inline float GetX() const { return m_X; }
        inline float GetY() const { return m_Y; }
        inline glm::vec2 GetPos() const { return { m_X, m_Y }; }

private:
        float m_X, m_Y;

};

class MouseButtonPressedEvent : public Event {
public:
        _EVENT_CLASS_CATEGORY(kMouse | kMouseButton);
        _EVENT_CLASS_TYPE(MouseButtonPressed);

        MouseButtonPressedEvent(MouseButton button) : m_Button(button) {}
        inline MouseButton GetButton() const { return m_Button; }
        operator std::string() const override
        {
                return std::format("Event {}: (BUTTON: {})",
                                   GetName(), magic_enum::enum_name(m_Button));
        }

private:
        MouseButton m_Button;
};

class MouseButtonReleasedEvent : public Event {
public:
        _EVENT_CLASS_CATEGORY(kMouse | kMouseButton);
        _EVENT_CLASS_TYPE(MouseButtonReleased);

        MouseButtonReleasedEvent(MouseButton button) : m_Button(button) {}
        inline MouseButton GetButton() const { return m_Button; }
        operator std::string() const override
        {
                return std::format("Event {}: (BUTTON: {})",
                                   GetName(), magic_enum::enum_name(m_Button));
        }

private:
        MouseButton m_Button;
};

class MouseScrolledEvent : public Event {
public:
        _EVENT_CLASS_CATEGORY(kMouse);
        _EVENT_CLASS_TYPE(MouseScrolled);

        MouseScrolledEvent(float amount_x, float amount_y)
                : m_AmountX(amount_x), m_AmountY(amount_y) {}

        inline float GetAmountX() const { return m_AmountX; }
        inline float GetAmountY() const { return m_AmountY; }
        operator std::string() const override
        {
                return std::format("Event {}: (DX: {} DY: {})",
                                   GetName(), m_AmountX, m_AmountY);
        }

private:
        float m_AmountX;
        float m_AmountY;
};

}  // namespace Borek
