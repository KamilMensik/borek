// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Base/Entity.h"
#include <cstdint>

#include "Include/Base/Input.h"
#include "Tools/Tilemap/ITilemapTool.h"

namespace Borek {

class BrushTool : public ITilemapTool {
public:
        bool
        OnMousePressed(MouseButton button) override;

        bool
        OnMouseReleased(MouseButton button) override;

        void
        Tick() override;

        void
        Activate() override;

        void
        Deactivate() override;

        void
        SetData(const ITilemapTool::DataPayload& payload) override;

private:
        enum class State { kNothing, kDrawing, kErasing }; 

        Entity m_CurrentEntity;
        uint32_t m_SelectedCell = UINT32_MAX;
        State m_State;
        bool m_Active = false;
};

}  // namespace Borek
