// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include <cstdint>

#include <mruby.h>
#include <mruby/hash.h>

#include "Include/Base/Application.h"
#include "Include/Engine/Alloc.h"
#include "Include/Components/FZXComponents.h"

namespace Borek {

static mrb_value hash_value = {0};
static mrb_value self_key_value;
static mrb_value other_key_value;

AreaComponent::AreaComponent()
        : colliding_with(NewAUniq<std::vector<uint32_t>>()),
        callbacks(NewAUniq<AreaMsgConnections>())
{
}

AreaComponent::AreaComponent(const AreaComponent& other) : AreaComponent()
{
        *colliding_with = *other.colliding_with;
        *callbacks = *other.callbacks;
        physics_flags = other.physics_flags;
        collision_mask = other.collision_mask;
        body_type = other.body_type;
        collider_type = other.collider_type;

        switch(other.collider_type) {
        case FZX::ColliderType::Rectangle:
                rc.size_x = other.rc.size_x;
                rc.size_y = other.rc.size_y;
                rc.rotation = other.rc.rotation;
                break;
        case FZX::ColliderType::Tilemap:
                tc.row = other.tc.row;
                tc.col = other.tc.col;
                break;
        case FZX::ColliderType::Circle:
                cc.radius = other.cc.radius;
                break;
        }
}

AreaComponent::AreaComponent(AreaComponent&& other)
{
        colliding_with = std::move(other.colliding_with);
        callbacks = std::move(other.callbacks);
        physics_flags = other.physics_flags;
        collision_mask = other.collision_mask;
        body_type = other.body_type;
        collider_type = other.collider_type;

        switch(other.collider_type) {
        case FZX::ColliderType::Rectangle:
                rc.size_x = other.rc.size_x;
                rc.size_y = other.rc.size_y;
                rc.rotation = other.rc.rotation;
                break;
        case FZX::ColliderType::Tilemap:
                tc.row = other.tc.row;
                tc.col = other.tc.col;
                break;
        case FZX::ColliderType::Circle:
                cc.radius = other.cc.radius;
                break;
        }
}

AreaComponent&
AreaComponent::operator=(const AreaComponent& other)
{
        *colliding_with = *other.colliding_with;
        *callbacks = *other.callbacks;
        physics_flags = other.physics_flags;
        collision_mask = other.collision_mask;
        body_type = other.body_type;
        collider_type = other.collider_type;

        switch(other.collider_type) {
        case FZX::ColliderType::Rectangle:
                rc.size_x = other.rc.size_x;
                rc.size_y = other.rc.size_y;
                rc.rotation = other.rc.rotation;
                break;
        case FZX::ColliderType::Tilemap:
                tc.row = other.tc.row;
                tc.col = other.tc.col;
                break;
        case FZX::ColliderType::Circle:
                cc.radius = other.cc.radius;
                break;
        }

        return *this;
}

AreaComponent&
AreaComponent::operator=(AreaComponent&& other)
{
        colliding_with = std::move(other.colliding_with);
        callbacks = std::move(other.callbacks);
        physics_flags = other.physics_flags;
        collision_mask = other.collision_mask;
        body_type = other.body_type;
        collider_type = other.collider_type;

        switch(other.collider_type) {
        case FZX::ColliderType::Rectangle:
                rc.size_x = other.rc.size_x;
                rc.size_y = other.rc.size_y;
                rc.rotation = other.rc.rotation;
                break;
        case FZX::ColliderType::Tilemap:
                tc.row = other.tc.row;
                tc.col = other.tc.col;
                break;
        case FZX::ColliderType::Circle:
                cc.radius = other.cc.radius;
                break;
        }

        return *this;
}

void
AreaComponent::OnEntityEntered(Entity self, Entity other)
{
        mrb_state* mrb = Application::GetRubyEngine().GetRubyVM();
        if (hash_value.w == 0) {
                hash_value = mrb_hash_new(mrb);
                self_key_value = mrb_symbol_value(mrb_intern_cstr(mrb, "from"));
                other_key_value = mrb_symbol_value(mrb_intern_cstr(mrb, "other"));
        }

        mrb_hash_set(mrb, hash_value, self_key_value, { self.GetRubyNode() });
        mrb_hash_set(mrb, hash_value, other_key_value, { other.GetRubyNode() });

        switch (other.GetNodeType()) {
        case NodeType::Area:
                callbacks->on_area_entered.Call(hash_value.w);
                break;
        case NodeType::StaticBody:
        case NodeType::DynamicBody:
        case NodeType::Tilemap:
                callbacks->on_body_entered.Call(hash_value.w);
                break;
        default:
                return;
        }
}

void
AreaComponent::OnEntityExited(Entity self, Entity other)
{
        uint64_t other_ruby_node = other.GetRubyNode();
        if (other_ruby_node == UINT64_MAX)
                return;

        mrb_state* mrb = Application::GetRubyEngine().GetRubyVM();
        mrb_hash_set(mrb, hash_value, self_key_value, { self.GetRubyNode() });
        mrb_hash_set(mrb, hash_value, other_key_value, { other_ruby_node });

        switch (other.GetNodeType()) {
        case NodeType::Area:
                callbacks->on_area_exited.Call(hash_value.w);
                break;
        case NodeType::StaticBody:
        case NodeType::DynamicBody:
        case NodeType::Tilemap:
                callbacks->on_body_exited.Call(hash_value.w);
                break;
        default:
                return;
        }
}

}  // namespace Borek
