// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Base/Layer.h"
namespace Borek {

class ProjectLayer : public Layer {
public:
        void OnAttach() override;
        void OnDetach() override;
        void OnUpdate(float delta) override;
        void OnImGuiRender() override;
private:

};

}  // namespace Borek
