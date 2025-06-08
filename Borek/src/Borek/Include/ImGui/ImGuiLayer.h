#pragma once

#include "Include/Base/Layer.h"

namespace Borek {

class ImGuiLayer : public Layer {
public:
        ImGuiLayer();
        ~ImGuiLayer();

        void OnAttach() override;
        void OnDetach() override;
        void OnImGuiRender() override;
        void OnEvent(Event &e) override;
        void RenderPrepare();
        void RenderFinish();
        void SetEventBlocking(bool val) { m_BlockEvents = val; }

        void SetDefaultColors();
private:
        bool m_BlockEvents;
};

}
