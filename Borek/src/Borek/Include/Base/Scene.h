// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "box2d/id.h"

#include <ECS/World.h>

#include "Include/Base/Entity.h"
#include "Include/Engine/Node.h"

#pragma once

namespace Borek {

class Scene {
friend Entity;
friend class SceneSerializer;
friend class Application;
public:
        Scene();
        ~Scene();

        Entity NewEntity(const std::string& tag = "");
        void DeleteEntity(Entity e);

public:
        ECS::World m_World;
        Node* m_RootNode;
        b2WorldId m_PhysicsWorld;
};

}  // namespace Borek
