// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Base/Layer.h"
#include "Include/Events/Event.h"
namespace Borek {

class EditorEventHandlerLayer : public Layer {
public:
        void OnAttach() override;
        void OnDetach() override;

private:
        std::array<EVHandle, 5> m_EventHandles;
};

}  // namespace Borek
