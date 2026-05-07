// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <string>
#include <variant>

#include "Include/Base/Scene.h"
#include "Include/Core.h"

namespace Borek {

class SceneSerializer {
public:
        SceneSerializer(Ref<Scene> scene);

        void
        Serialize(const std::filesystem::path& path, Entity e = Entity());

        void
        Deserialize(const std::filesystem::path& path, Entity e = Entity());

        void
        AddMsgConnectionCommand(std::string_view path,
                                struct MsgConnection* connection,
                                uint32_t entity);

        void
        HandleCommands();

        bool
        IsSerializingPrefab();

        void
        SetSerializingPrefab(bool val);

private:
        struct InitMsgConnection {
                std::string path;
                struct MsgConnection* connection;
                uint32_t entity;
        };

        struct Command {
                std::variant<InitMsgConnection> value;
        };

        using CommandVec = std::vector<Command>;
        Ref<Scene> m_Scene;
        CommandVec m_Commands;
        bool m_SerializingPrefab;
};

}  // namespace Borek
