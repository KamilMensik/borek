// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Base/Popup.h"

namespace Borek {
namespace Popups {

class EditorSettingsPopup : public Popup {
public:
        EditorSettingsPopup() = default;

        bool Tick() override;
};

}  // namespace Popups
}  // namespace Borek
