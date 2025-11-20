// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Base/Layer.h"
#include "Include/Events/Event.h"
#include <array>

namespace Borek {

class ResourceLayer : public Layer {
public:
        void OnUpdate(float delta) override;
        void OnAttach() override;
        void OnDetach() override;

private:
        int m_TickCounter = 30;
        std::array<EVHandle, 1> m_EventHandles;
};

}  // namespace Borek
