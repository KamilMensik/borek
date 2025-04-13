#pragma once

#include "./Application.h"
#include "./Log.h"

extern Borek::Application* Borek::CreateApplication();

int main(int argc, char** argv)
{
        Borek::Log::Init();
        auto app = Borek::CreateApplication();
        app->Run();
        delete app;

        return 0;
}
