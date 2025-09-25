// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <cstdint>
#include <string>

#include "Include/Base/Popup.h"

namespace Borek {
namespace Popups {

class AddScriptPopup : public Popup {
public:
        AddScriptPopup(uint32_t entity_id);

        bool Tick() override;

private:
        std::string m_SelectedClass = "";
        std::string m_Search = "";
        uint32_t m_EntityId;
};

}  // namespace Popups
}  // namespace Borek
