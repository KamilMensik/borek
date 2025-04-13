#pragma once

#include "Include/Layer.h"

namespace Borek {

class ImGuiLayer : public Layer {
public:
        ImGuiLayer();
        ~ImGuiLayer();

        void OnAttach() override;
        void OnDetach() override;
        void OnImGuiRender() override;
        void RenderPrepare();
        void RenderFinish();
};

}
