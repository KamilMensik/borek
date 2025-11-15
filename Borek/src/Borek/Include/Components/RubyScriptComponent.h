// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Base/Entity.h"
#include "Include/Engine/Assets/Asset.h"
#include "Include/Engine/Assets/ScriptAsset.h"

namespace Borek {

struct RubyScriptComponent {
        RubyScriptComponent();

        void Initialize(Entity e);
        void OnUpdate(float delta);
        void OnDestroy();

        uint64_t ruby_instance = 0;
        Asset<ScriptAsset> script;

        static class RClass* s_RubyEntityClass;
};

}  // namespace Borek
