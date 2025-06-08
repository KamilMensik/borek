// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Base/Entity.h"
#include "Include/Events/Event.h"

namespace Borek {

class ScriptableObject {
friend class Scene;
friend struct ScriptComponent;
public:
        ScriptableObject();
        virtual void OnCreate();
        virtual void OnUpdate(float delta);
        virtual void OnDestroy();
        virtual void OnEvent(Event& e);

private:
        Entity m_Entity;

public:
        template <class T>
        inline T& GetComponent()
        {
                return m_Entity.GetComponent<T>();
        }
};

}  // namespace Borek
