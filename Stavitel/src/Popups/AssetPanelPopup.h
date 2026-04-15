// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Base/Popup.h"

#include <filesystem>

namespace Borek {
namespace Popups {

class AssetPanelPopup : public Popup {
public:
        enum class State : uint8_t {
                kNormal,
                kCreateDir,
                kFileContextMenu,
                kFileRename,
                kCreateScene,
        };

        AssetPanelPopup(const std::filesystem::path& context_path, State s);

        bool Tick() override;

private:
        std::filesystem::path m_ContextPath;
        std::string m_Input;
        State m_State;
};

}  // namespace Popups
}  // namespace Borek
