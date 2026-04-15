// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <cstdint>

#include "Include/Base/MsgConnection.h"
#include "Include/Scripting/Ruby/Modules/RBModule.h"
#include "mrbcpp.h"

namespace Borek {
namespace RBModules {

class RBMsgConnection : RBModule {
public:
        struct Data {
                MsgConnection* connection;
        };

        static void Init(class RubyEngine& vm);

        static mrbcpp::Class msg_connection_class;
};

}  // namespace RBModules
}  // namespace Borek
