// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <functional>
#include <string>

#include "Include/Core.h"
#include "Include/Events/Event.h"

namespace Borek
{

class AbstractWindow {
    public:
        using CallbackFn = std::function<void(Event*)>;

        AbstractWindow(unsigned width, unsigned height,
                       const std::string &name)
                : m_Width(width)
                , m_Height(height)
                , m_Title(name)
        {
        }

        virtual ~AbstractWindow() {};
        virtual void OnUpdate() = 0;

        inline unsigned GetWidth() const
        {
                return m_Width;
        }
        inline unsigned GetHeight() const
        {
                return m_Height;
        }
        const inline std::string &GetTitle() const
        {
                return m_Title;
        }

        virtual Time GetTime() = 0;

        virtual void SetCallback(const CallbackFn &callback) = 0;
        virtual void* WindowImpl() = 0;

        virtual void SetVsync(bool value) = 0;

        virtual bool GetVsync() = 0;

        inline bool IsMinimized() { return (m_Width + m_Height) == 0; }

    protected:
        unsigned m_Width;
        unsigned m_Height;
        std::string m_Title;
};

} // namespace Borek
