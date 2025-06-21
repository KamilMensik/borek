// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <filesystem>

namespace Borek {
namespace Panels {

class Assets {
public:
        Assets();
        void OnImguiRender();

private:
        void FileButton(const std::filesystem::directory_entry& file, float size);
        std::filesystem::path m_CurrentPath;
};

}  // namespace Panels
}  // namespace Borek
