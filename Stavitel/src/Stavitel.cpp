// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include <Borek.h>

#include "Layers/EditorLayer.h"

namespace Borek {

static Ref<Graphics::FrameBuffer> fb;

class Stavitel : public Application
{
public:
        Stavitel() : Borek::Application("Borek Stavitel!")
        {
                Graphics::FrameBufferSettings fb_settings;
                fb_settings.width = 1280;
                fb_settings.height = 720;
                fb = Graphics::FrameBuffer::Create(fb_settings);

                m_EditorLayer = new EditorLayer(fb);
                PushLayer(m_EditorLayer);
        }

        ~Stavitel()
        {
        }

        void OnUpdate(float delta) override
        {
        }

        void OnRenderBegin() override
        {
                m_EditorLayer->SetGameCamera();
                fb->Bind();
                m_EditorLayer->BeginDockspace();
        }

        void OnRenderEnd() override
        {
                fb->Unbind();
                m_EditorLayer->EndDockspace();
        }

        EditorLayer* m_EditorLayer;
        bool m_Playing;
};

}  // namespace Borek

BOREK_APPLICATION(Borek::Stavitel)
