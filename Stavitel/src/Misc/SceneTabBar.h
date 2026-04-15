// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Commands/ICommand.h"
#include <concepts>
#include <cstdint>
#include <filesystem>
#include <vector>

#include "Include/Base/Scene.h"
#include "Include/Core.h"
#include "Commands/CommandHandler.h"

namespace Borek {

class SceneTabBar {
public:
        static void
        Deinit();

        static void
        ChangeScene(Ref<Scene> scene);

        static void
        ChangeSceneEmpty();

        static void
        ChangeScene(const std::filesystem::path& path, bool force = false);

        static void
        RuntimeChangeScene(Ref<Scene> new_scene);

        static void
        RemoveScene(Ref<Scene> scene);

        static void
        SaveScene();

        static void
        OnImguiRender();

        static CommandHandler&
        GetCommandHandler();

        static const Ref<Scene>&
        GetCurrentScene();

        template <class T, typename ... Args> requires std::derived_from<T, ICommand>
        static void
        SendCommand(Args ... args)
        {
                Uniq<T> cmd = NewUniq<T>(args...);
                GetCommandHandler().AddCommand(std::move(cmd));
        }

private:
        struct SceneData {
                Ref<Scene> scene;
                Uniq<CommandHandler> cmd_handler;
                bool is_open;
                bool is_modified;

                bool
                operator ==(const SceneData& other) const
                {
                        return scene == other.scene;
                }
        };

        static void
        DisableScene(Ref<Scene> scene);

        static void
        EnableScene(Ref<Scene> scene);

        static void
        SwapScene(Ref<Scene> scene);

        static std::vector<SceneData> s_OpenedScenes;
        static StringMap<uint32_t> s_OpenedScenesByName;
        static uint32_t s_CurrentScene;
        static uint32_t s_ForceSelectScene;
};

}  // namespace Borek
