// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Events/Event.h"
#include <cstdint>

namespace Borek {

class ComponentAddedEvent : public Event {
_BASE_EVENT_HEADER(ComponentAddedEvent)
public:
        ComponentAddedEvent(uint32_t id, uint32_t entity_id) :
                m_EntityId(entity_id), m_Id(id) {}

        inline uint32_t GetEntityId() { return m_EntityId; }
        inline uint32_t GetId() { return m_Id; }

private:
        uint32_t m_EntityId;
        uint32_t m_Id;
};

class ComponentRemovedEvent : public Event {
_BASE_EVENT_HEADER(ComponentRemovedEvent)
public:
        ComponentRemovedEvent(uint32_t id, uint32_t entity_id) : m_EntityId(entity_id), m_Id(id) {}

        inline uint32_t GetEntityId() { return m_EntityId; }
        inline uint32_t GetId() { return m_Id; }

private:
        uint32_t m_EntityId;
        uint32_t m_Id;
};

}  // namespace Borek
