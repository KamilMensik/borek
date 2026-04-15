// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Scripting/Ruby/Modules/RBModule.h"
#include "mrbcpp.h"

#define IS_ASSET_TYPE(_val, _cls)                                  \
        (MRB_INSTANCE_TT(RCAST<RClass*>(_val.w)->c) == MRB_TT_DATA && \
        ((RClass*)RBAsset::_cls##_class) == ((RClass*)_val.w)->c)

#define ASSET_TYPE_ASSERT(_val, _cls)                                          \
        if (!IS_ASSET_TYPE(_val, _cls)) {                                                  \
                mrb_raisef(mrb, E_ARGUMENT_ERROR, "Invalid asset type");       \
                return MRB_NIL;                                                \
        }

#define GET_ASSET_ID(_val) \
        reinterpret_cast<uint64_t>(DATA_PTR(_val))

namespace Borek {
namespace RBModules {

class RBAsset : RBModule {
public:
        static void Init(class RubyEngine& vm);

        static mrbcpp::Class scene_class;
        static mrbcpp::Class tex_class;
        static mrbcpp::Class sound_class;
        static mrbcpp::Class spritesheet_class;
        static mrbcpp::Class tilemap_class;
        static mrbcpp::Class animation_class;
        static mrbcpp::Class script_class;
        static mrbcpp::Class font_class;
};

}  // namespace RBModules
}  // namespace Borek
