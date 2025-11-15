// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Base/Entity.h"
#include <Include/Engine/Assets/TilemapAsset.h>

#include "Tools/ITool.h"
#include <cstdint>

namespace Borek {

class ITilemapTool : public ITool {
public:
        struct DataPayload {
                Asset<TilemapAsset> tmap;
                Entity entity;
                uint32_t selected_cell;
        };

        virtual void
        SetData(const DataPayload& data) = 0;
};

}  // namespace Borek
