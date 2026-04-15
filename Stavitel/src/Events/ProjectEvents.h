// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <filesystem>

#include "Include/Events/Event.h"

namespace Borek {

class SwitchProjectEvent : public Event {
_BASE_EVENT_HEADER(SwitchProjectEvent);
public:
        SwitchProjectEvent(const std::filesystem::path& path)
                : m_Path(path) {}
        inline const std::filesystem::path&
        GetPath() { return m_Path; }

private:
        std::filesystem::path m_Path;
};

}  // namespace Borek
