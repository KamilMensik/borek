// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Scripting/ScriptableObject.h"

namespace Borek {

struct ScriptComponent {
        ScriptableObject* instance;

        ScriptComponent() {}
        ScriptComponent(ScriptableObject* so) { instance = so; }

        template <class T> requires std::derived_from<T, ScriptableObject>
        inline void SetScript() { SetScript(new T()); }
        inline void SetScript(ScriptableObject* obj) { instance = obj; }

        inline void Bind(Entity e)
        {
                if (!instance->m_Entity.IsNil()) return;

                instance->m_Entity = e;
                instance->OnCreate();
        }
};

}  // namespace Borek
