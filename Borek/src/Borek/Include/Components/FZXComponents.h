// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Base/Entity.h"
#include "Include/Base/MsgConnection.h"
#include "Include/Engine/Alloc.h"
#include "Include/Engine/FZX/Body.h"
#include <cstdint>

namespace Borek {

using BodyComponent = FZX::Body;

struct AreaMsgConnections {
        MsgConnection on_body_entered;
        MsgConnection on_body_exited;
        MsgConnection on_area_entered;
        MsgConnection on_area_exited;
};

struct AreaComponent : public BodyComponent {
        AreaComponent();
        AreaComponent(const AreaComponent& other);
        AreaComponent(AreaComponent&& other);
        AreaComponent& operator=(const AreaComponent& other);
        AreaComponent& operator=(AreaComponent&& other);

        void OnEntityEntered(Entity self, Entity other);
        void OnEntityExited(Entity self, Entity other);

        AUniq<std::vector<uint32_t>> colliding_with;
        AUniq<AreaMsgConnections> callbacks;
};

}  // namespace Borek
