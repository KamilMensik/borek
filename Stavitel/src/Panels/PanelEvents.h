// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Base/Entity.h"
#include "Include/Events/Event.h"

namespace Borek {

class ChangeEntityEvent : public Event {
_BASE_EVENT_HEADER(ChangeEntityEvent);
public:
        ChangeEntityEvent(Entity e) : m_Entity(e) {}

        inline Entity GetEntity() { return m_Entity; }
private:
        Entity m_Entity;
};

}  // namespace Borek
