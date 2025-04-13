// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <format>

#include "./Event.h"

namespace Borek {

class WindowResizeEvent : public Event {
public:
        _EVENT_CLASS_CATEGORY(kWindow);
        _EVENT_CLASS_TYPE(WindowResize);

        WindowResizeEvent(unsigned int width, unsigned int height) :
                m_Width(width), m_Height(height) {}

        inline unsigned int GetWidth() const { return m_Width; }
        inline unsigned int GetHeight() const { return m_Height; }

        operator std::string() const override
        {
                return std::format("Event {}: (NEW_WIDTH: {} NEW_HEIGHT: {})",
                                   GetName(), m_Width, m_Height);
        }

private:
        unsigned int m_Width, m_Height;
};

class WindowCloseEvent : public Event {
public:
        _EVENT_CLASS_CATEGORY(kWindow);
        _EVENT_CLASS_TYPE(WindowClose);

        WindowCloseEvent() {}
};

class WindowFocusEvent : public Event {
public:
        _EVENT_CLASS_CATEGORY(kWindow);
        _EVENT_CLASS_TYPE(WindowFocus);

        WindowFocusEvent() {}
};

class WindowLostFocusEvent : public Event {
public:
        _EVENT_CLASS_CATEGORY(kWindow);
        _EVENT_CLASS_TYPE(WindowLostFocus);

        WindowLostFocusEvent() {}
};

class WindowMoveEvent : public Event {
public:
        _EVENT_CLASS_CATEGORY(kWindow);
        _EVENT_CLASS_TYPE(WindowResize);

        WindowMoveEvent(float x , float y) :
                m_X(x), m_Y(y) {}

        inline unsigned int GetX() const { return m_X; }
        inline unsigned int GetY() const { return m_Y; }

        operator std::string() const override
        {
                return std::format("Event {}: (X: {} Y: {})",
                                   GetName(), m_X, m_Y);
        }

private:
        unsigned int m_X, m_Y;
};

}  // namespace Borek
