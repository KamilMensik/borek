// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <cstdint>
#include <string>

#include "Include/Base/Popup.h"
#include "Include/Base/Entity.h"
#include "Include/Base/MsgConnection.h"

namespace Borek {
namespace Popups {

class MsgConnectionPopup : public Popup {
public:
        MsgConnectionPopup(uint32_t entity_id, MsgConnection* connection);

        bool Tick() override;

private:
        std::string m_Search = "";
        std::string m_FunctionName = "";
        MsgConnection* m_Connection;
        Entity m_Entity;
        Entity m_SelectedEntity;
};

}  // namespace Popups
}  // namespace Borek
