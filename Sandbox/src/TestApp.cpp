#include "Include/Objects/SpriteSheet.h"
#include <Borek.h>

#include "Include/Base/Renderer2D.h"
#include "Include/Base/Colors.h"

class TestLayer : public Borek::Layer {
public:
        TestLayer() : Borek::Layer("TestLayer")
        {
                m_UnknownPlumberTexture = Borek::Graphics::Texture2D::Create(ASSET_PATH("assets/retro-mario.png"));
                m_SpriteSheet = Borek::SpriteSheet::Create(ASSET_PATH("assets/tilemap.png"));
                m_SpriteSheet->SetStep(16, 16);
                m_SpriteSheet->SetGap(1);
                //m_Subsprite = Borek::SubSprite::Create(m_SpriteSheet);
        }

        ~TestLayer() override {}

        void OnUpdate(float delta) override
        {
                BOREK_ENGINE_TRACE("FPS: {}", 1 / delta);

                Borek::Renderer2D::DrawQuad({ 0.0f, 0.0f }, { 64, 64 },
                                            Borek::Colors::RED);
                Borek::Renderer2D::DrawQuad({ 0.0f, -64 }, { 64, 64 },
                                            Borek::Colors::GREEN);
                Borek::Renderer2D::DrawQuad({ -64, -64 }, { 64, 64 },
                                            Borek::Colors::BLUE);
                Borek::Renderer2D::DrawQuad({ -64, 0.0f }, { 64, 64 },
                                            Borek::Colors::WHITE);
                Borek::Renderer2D::DrawQuad({ -128, -128 }, { 64, 64 },
                                            Borek::Colors::RED);
                Borek::Renderer2D::DrawQuad({ -128, -128 }, { 64, 64 },
                                            Borek::Colors::GREEN);
                Borek::Renderer2D::DrawQuad({ -64, 0.0f }, { 64, 64 },
                                            Borek::Colors::BLUE);
                Borek::Renderer2D::DrawQuad({ -128, -64 }, { 64, 64 },
                                            Borek::Colors::WHITE);

                //for (int i = 0; i < 12; i++) {
                //        for (int j = 0; j < 11; j++) {
                //                m_Subsprite->SetRowCol(i, j);
                //                Borek::Renderer2D::DrawQuad({ i * 64, j * 64 }, { 64, 64 },
                //                                            m_Subsprite);
                //        }
                //}
        }

        void OnEvent(Borek::Event& e) override
        {
                //BOREK_LOG_INFO(std::string(e));
        }
private:
        Ref<Borek::Graphics::Texture> m_UnknownPlumberTexture;
        Ref<Borek::SpriteSheet> m_SpriteSheet;
        Ref<Borek::SubSprite> m_Subsprite;
};

class Test : public Borek::Application
{
public:
        Test() : Borek::Application()
        {
                PushLayer(new TestLayer());
        }

        ~Test()
        {
        }

        void OnUpdate(float delta) override
        {
        }
};

BOREK_APPLICATION(Test)
