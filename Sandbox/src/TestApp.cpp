#include <Borek.h>
#include "Borek/Include/Log.h"
#include "GLFW/glfw3.h"

class TestLayer : public Borek::Layer {
public:
        TestLayer() : Borek::Layer("TestLayer")
        {
        }

        ~TestLayer() override {}

        void OnUpdate() override
        {
                double delta = glfwGetTime() - time;
                frame_count++;
                if (delta > 0.25) {
                        time += delta;
                        BOREK_ENGINE_TRACE("FPS: {}", frame_count / delta);
                        frame_count = 0;
                }
        }
        void OnEvent(Borek::Event& e) override
        {
                //BOREK_LOG_INFO(std::string(e));
        }

private:
        unsigned int frame_count;
        double time;
};

class Test : public Borek::Application
{
public:
        Test()
        {
                PushLayer(new TestLayer());
        }

        ~Test()
        {
        }
};

Borek::Application* Borek::CreateApplication()
{
        return new Test();
}
