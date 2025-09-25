// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <string>
#include <vector>

#include <glm/ext/vector_float4.hpp>

namespace Borek {
namespace Panels {

class ConsolePanel {
public:
        ConsolePanel() = default;
        void OnImguiRender();
        void Add(const std::string& str, const glm::vec4& color);
        void Reset();

private:
        std::vector<std::pair<std::string, glm::vec4>> m_Messages;
};

}  // namespace Panels
}  // namespace Borek
