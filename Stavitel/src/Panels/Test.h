// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Objects/Font.h"

namespace Borek {
namespace Panels {

class Test {
public:
        Test();
        void OnImguiRender();

private:
        Font m_Font;
};

}  // namespace Panels
}  // namespace Borek
