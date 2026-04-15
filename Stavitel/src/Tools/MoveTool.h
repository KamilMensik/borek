// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Components/TransformComponent.h"
#include <Borek/Include/Base/Entity.h>
#include <Borek/Include/Base/Input.h>

#include "Tools/IToolbarTool.h"


namespace Borek {

class MoveTool : public IToolbarTool{
public:
        void
        OnMousePressed(MouseButton button) override;

        void
        OnMouseReleased(MouseButton button) override;

        bool
        Tick() override;

        void
        Activate() override;

        void
        Deactivate() override;

        void
        SetEntity(Entity e) override;

private:
        enum class State { kDisabled, kNothing, kXAxis, kYAxis, kXYAxis, };

        Entity m_CurrentEntity;
        State m_State = State::kDisabled;
        glm::vec2 m_Offset;
        TransformComponent m_OldTransform;
};

}  // namespace Borek
