// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Events/Event.h"

namespace Borek {

class WindowResizeEvent : public Event {
_BASE_EVENT_HEADER(WindowResizeEvent)
public:
        WindowResizeEvent(unsigned int width, unsigned int height) :
                m_Width(width), m_Height(height) {}

        inline unsigned int GetWidth() const { return m_Width; }
        inline unsigned int GetHeight() const { return m_Height; }

private:
        unsigned int m_Width, m_Height;
};

class WindowCloseEvent : public Event {
_BASE_EVENT_HEADER(WindowCloseEvent)
public:
        WindowCloseEvent() {}
};

class WindowFocusEvent : public Event {
_BASE_EVENT_HEADER(WindowFocusEvent)
public:
        WindowFocusEvent() {}
};

class WindowLostFocusEvent : public Event {
_BASE_EVENT_HEADER(WindowLostFocusEvent)
public:
        WindowLostFocusEvent() {}
};

class WindowMoveEvent : public Event {
_BASE_EVENT_HEADER(WindowMoveEvent)
public:
        WindowMoveEvent(float x , float y) :
                m_X(x), m_Y(y) {}

        inline unsigned int GetX() const { return m_X; }
        inline unsigned int GetY() const { return m_Y; }

private:
        unsigned int m_X, m_Y;
};

}  // namespace Borek
