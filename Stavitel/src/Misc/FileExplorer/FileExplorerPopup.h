// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <cstdint>
#include <filesystem>
#include <string>

#include "Include/Base/Popup.h"

namespace Borek {
namespace Popups {

class FileExplorerPopup : public Popup {
public:
        enum class State : uint8_t {
                kNormal,
                kCreateDir,
                kFileContextMenu,
                kFileRename,
        };

        FileExplorerPopup(const std::filesystem::path& context_path, State s);

        bool
        Tick() override;

private:
        std::filesystem::path m_ContextPath;
        std::string m_Input;
        State m_State;
        bool m_Initialized = false;
};

}  // namespace Popups
}  // namespace Borek
