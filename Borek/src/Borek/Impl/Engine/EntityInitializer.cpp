// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Base/Application.h"
#include "Include/Base/Entity.h"
#include "Include/Debug/Log.h"
#include "Include/Engine/FZX/Body.h"
#include "Include/Graphics/Camera.h"

#include "Include/Engine/EntityInitializer.h"
#include "Include/Base/Components.h"

namespace Borek {

void
EntityInitializer::InitializeBegin(Entity e)
{
        s_GlobalTransform += e.GetComponent<TransformComponent>();

        switch (e.GetNodeType()) {
        case NodeType::Node:
        case NodeType::Camera:
        case NodeType::Text:
        case NodeType::Sprite:
                break;
        case NodeType::DynamicBody:
        case NodeType::StaticBody:
        case NodeType::Area:
                InitializeFZXBody(e);
                break;
        }
}

void
EntityInitializer::InitializeEnd(Entity e)
{
        s_GlobalTransform -= e.GetComponent<TransformComponent>();
}

void
EntityInitializer::InitializeFZXBody(Entity e)
{
        auto& body = e.GetComponent<FZX::BodyComponent>();

        body.Update(s_GlobalTransform.position, s_GlobalTransform.scale);
        Application::GetScene()->m_PhysicsWorld.Add(e, body);
}

TransformComponent EntityInitializer::s_GlobalTransform = TransformComponent();
}  // namespace Borek

