// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Debug/Log.h"
#include "Include/Engine/Assets/Asset.h"
#include "Include/Engine/Assets/SceneAsset.h"
#include "Include/Scripting/Ruby/Modules/RBAsset.h"
#include <mruby.h>
#include <mruby/value.h>
#include <mruby/string.h>
#include <mruby/variable.h>
#include <mruby/value.h>
#include <mruby/data.h>
#include <mruby/class.h>

#include <mrbcpp.h>
#include <ECS/Component.h>

#include "Include/Scripting/Ruby/RubyEngine.h"
#include "Include/Scripting/Ruby/Modules/RBScene.h"
#include "Include/Base/Application.h"
#include <string_view>

namespace Borek {
namespace RBModules {

using namespace mrbcpp;

static MRB_FUNC(Root)
{
        return { Application::GetScene()->GetTree().GetRootEntity().GetRubyNode() };
}

static MRB_FUNC(Change) {
        mrb_value arg = MRB_ARG1;
        ASSET_TYPE_ASSERT(arg, scene);

        Asset<SceneAsset> ass(RCAST<uint64_t>(DATA_PTR(arg)));
        auto path = std::string_view(ass->scene_path).substr(0, ass->scene_path.size() - 1);
        Application::SendEvent<ChangeSceneEvent>(path);
        return MRB_NIL;
}

static MRB_FUNC(Restart) {
        Application::SendEvent<ChangeSceneEvent>(Application::GetScene()->GetPath().string());
        return MRB_NIL;
}

void Scene::Init(RubyEngine& engine)
{
        Module& borek = engine.GetBorekModule();

        borek.define_class("Scene")
                .define_class_method("root", Root)
                .define_class_method("change", Change, FuncArgs().Required(1))
                .define_class_method("restart", Restart);
}

}  // namespace RBModules
}  // namespace Borek
