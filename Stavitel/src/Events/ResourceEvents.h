// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <Borek/Include/Events/Event.h>
#include <filesystem>

namespace Borek {

class ResourceRenamedEvent : public Event {
_BASE_EVENT_HEADER(ResourceRenamedEvent);
public:
        ResourceRenamedEvent(const std::filesystem::path& from,
                             const std::filesystem::path& to)
                : m_From(from), m_To(to) {}

        inline const std::filesystem::path&
        GetFrom() { return m_From; }

        inline const std::filesystem::path&
        GetTo() { return m_To; }

private:
        std::filesystem::path m_From, m_To;
};

class ResourceDeletedEvent : public Event {
_BASE_EVENT_HEADER(ResourceDeletedEvent);
public:
        ResourceDeletedEvent(const std::filesystem::path& path)
                : m_Path(path) {}

        inline const std::filesystem::path&
        GetPath() { return m_Path; }

private:
        std::filesystem::path m_Path;
};

}  // namespace Borek
