// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <glm/ext/vector_float2.hpp>

#include "Include/Base/Input.h"
#include "Include/Events/Event.h"

namespace Borek {

class MouseMovedEvent : public Event {
_BASE_EVENT_HEADER(MouseMovedEvent)
public:
        MouseMovedEvent(float x, float y) : m_X(x), m_Y(y) {}

        inline float GetX() const { return m_X; }
        inline float GetY() const { return m_Y; }
        inline glm::vec2 GetPos() const { return { m_X, m_Y }; }

private:
        float m_X, m_Y;
};

class MouseButtonEvent : public Event {
_BASE_EVENT_HEADER(MouseButtonEvent)
public:
        MouseButtonEvent(MouseButton button, bool pressed)
                : m_Button(button), m_Pressed(pressed) {}
        inline MouseButton GetButton() const { return m_Button; }
        inline bool IsPressed() const { return m_Pressed; }

private:
        MouseButton m_Button;
        bool m_Pressed;
};

class MouseScrolledEvent : public Event {
_BASE_EVENT_HEADER(MouseScrolledEvent)
public:
        MouseScrolledEvent(float amount_x, float amount_y)
                : m_AmountX(amount_x), m_AmountY(amount_y) {}

        inline float GetAmountX() const { return m_AmountX; }
        inline float GetAmountY() const { return m_AmountY; }

private:
        float m_AmountX;
        float m_AmountY;
};

}  // namespace Borek
