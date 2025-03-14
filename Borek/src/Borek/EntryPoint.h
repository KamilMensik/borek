#pragma once

#include "Application.h"

extern Borek::Application* Borek::CreateApplication();

int main(int argc, char** argv)
{
        auto app = Borek::CreateApplication();
        app->Run();
        delete app;

        return 0;
}
