// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Window.h"

namespace Borek {

namespace Graphics {

class Context {
public:
        virtual ~Context() {};
        static void Create(AbstractWindow* window);

        virtual void Init() = 0;
        virtual void SwapBuffers() = 0;

        static Context* Instance() {
                return s_Instance;
        }

protected:
        static Context* s_Instance;
};

}

}  // namespace Borek
