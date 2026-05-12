// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Debug/Log.h"
#include <cstdint>
#include <fstream>
#include <string>
#include <string_view>
#include <variant>
#include <utility>

#include <yaml-cpp/yaml.h>
#include <yaml-cpp/emitter.h>
#include <yaml-cpp/emittermanip.h>
#include <ECS/World.h>
#include <magic_enum/magic_enum.hpp>
#include <mrbcpp.h>

#include "Include/Engine/SceneSerializer.h"
#include "Include/Base/UUID.h"
#include "Include/Graphics/Camera.h"
#include "Include/Components/TransformComponent.h"
#include "Include/Components/IDComponent.h"
#include "Include/Components/Text2DComponent.h"
#include "Include/Components/SpriteComponent.h"
#include "Include/Components/CameraComponent.h"
#include "Include/Components/RubyScriptComponent.h"
#include "Include/Base/Application.h"
#include "Include/Base/Colors.h"
#include "Include/Base/Entity.h"
#include "Include/Base/Node.h"
#include "Include/Components/PrefabComponent.h"
#include "Include/Components/SoundplayerComponent.h"
#include "Include/Components/TilemapComponent.h"
#include "Include/Components/AnimatedSpriteComponent.h"
#include "Include/Engine/Assets/Asset.h"
#include "Include/Engine/Assets/ScriptAsset.h"
#include "Include/Engine/Assets/SoundAsset.h"
#include "Include/Engine/Assets/TilemapAsset.h"
#include "Include/Engine/Assets/SceneAsset.h"
#include "Include/Base/MsgConnection.h"
#include "Include/Base/Symbol.h"
#include "Include/Components/FZXComponents.h"
#include "Include/Components/ParticleComponent.h"
#include "Include/Components/TagComponent.h"
#include "Include/Components/ValueComponent.h"
#include "Include/Engine/Utils/StringHelpers.h"

template<class... Ts>
struct overloads : Ts... { using Ts::operator()...; };

namespace YAML {

template<>
struct convert<glm::vec3> {
        static Node encode(const glm::vec3& in)
        {
                Node node;
                node.push_back(in.x);
                node.push_back(in.y);
                node.push_back(in.z);
                return node;
        }

        static bool decode(const Node& node, glm::vec3& out)
        {
                if (!node.IsSequence() || node.size() != 3) return false;

                out.x = node[0].as<float>();
                out.y = node[1].as<float>();
                out.z = node[2].as<float>();

                return true;
        }
};

template<>
struct convert<glm::vec4> {
        static Node encode(const glm::vec4& in)
        {
                Node node;
                node.push_back(in.x);
                node.push_back(in.y);
                node.push_back(in.z);
                node.push_back(in.w);
                return node;
        }

        static bool decode(const Node& node, glm::vec4& out)
        {
                if (!node.IsSequence() || node.size() != 4) return false;

                out.x = node[0].as<float>();
                out.y = node[1].as<float>();
                out.z = node[2].as<float>();
                out.w = node[3].as<float>();

                return true;
        }
};

template <>
struct convert<glm::vec2> {
        static Node encode(const glm::vec2& in)
        {
                Node node;
                node.push_back(in.x);
                node.push_back(in.y);
                return node;
        }

        static bool decode(const Node& node, glm::vec2& out)
        {
                if (!node.IsSequence() || node.size() != 2) return false;

                out.x = node[0].as<float>();
                out.y = node[1].as<float>();

                return true;
        }
};

template <>
struct convert<glm::u16vec2> {
        static Node encode(const glm::u16vec2& in)
        {
                Node node;
                node.push_back(in.x * (1.0f / 64.0f));
                node.push_back(in.y * (1.0f / 64.0f));
                return node;
        }

        static bool decode(const Node& node, glm::u16vec2& out)
        {
                if (!node.IsSequence() || node.size() != 2) return false;

                out.x = node[0].as<float>() * 64.0f;
                out.y = node[1].as<float>() * 64.0f;

                return true;
        }
};

template <>
struct convert<glm::i16vec2> {
        static Node encode(const glm::i16vec2& in)
        {
                Node node;
                node.push_back(in.x * (1.0f / 64.0f));
                node.push_back(in.y * (1.0f / 64.0f));
                return node;
        }

        static bool decode(const Node& node, glm::i16vec2& out)
        {
                if (!node.IsSequence() || node.size() != 2) return false;

                out.x = node[0].as<float>() * 64.0f;
                out.y = node[1].as<float>() * 64.0f;

                return true;
        }
};

template <>
struct convert<Borek::Color> {
        static Node encode(const Borek::Color& in)
        {
                Node node;

                node.push_back(in.g);
                node.push_back(in.b);
                node.push_back(in.a);
                return node;
        }

        static bool decode(const Node& node, Borek::Color& out)
        {
                if (!node.IsSequence() || node.size() != 4) return false;

                out.r = node[0].as<uint8_t>();
                out.g = node[1].as<uint8_t>();
                out.b = node[2].as<uint8_t>();
                out.a = node[3].as<uint8_t>();

                return true;
        }
};

template <>
struct convert<Borek::Symbol> {
        static Node encode(const Borek::Symbol& in)
        {
                return Node(in.Str());
        }

        static bool decode(const Node& node, Borek::Symbol& out)
        {
                if (!node.IsScalar()) return false;

                out = node.as<std::string_view>();

                return true;
        }
};

template <>
struct convert<Borek::MsgConnection> {
        static Node
        encode(const Borek::MsgConnection& in, Borek::Entity e)
        {
                Node node;
                mrb_state* mrb = Borek::Application::GetRubyEngine().GetRubyVM();

                node["TargetPath"] = e.PathTo(in.entity_id);
                node["Callback"] = mrb_sym_name(mrb, in.rbsym);

                return node;
        }

        static bool
        decode(const Node& node, Borek::Entity e, Borek::MsgConnection& out)
        {
                if (!node.IsSequence() || node.size() != 4) return false;

                out.entity_id = e.FindEntityByPath(node["TargetPath"].as<std::string_view>());
                out.rbsym = Borek::Symbol(node["Callback"].as<std::string_view>());

                return true;
        }
};

static Emitter& operator<<(Emitter& out, const glm::vec3& in)
{
        out << Flow;
        out << BeginSeq << in.x << in.y << in.z << EndSeq;
        return out;
}

static Emitter& operator<<(Emitter& out, const glm::vec4& in)
{
        out << Flow;
        out << BeginSeq << in.r << in.g << in.b << in.a << EndSeq;
        return out;
}

static Emitter& operator<<(Emitter& out, const glm::vec2& in)
{
        out << Flow;
        out << BeginSeq << in.r << in.g << EndSeq;
        return out;
}

static Emitter& operator<<(Emitter& out, const Borek::Color& in)
{
        out << Flow;
        out << BeginSeq << int(in.r) << int(in.g) << int(in.b) << int(in.a) << EndSeq;
        return out;
}

static Emitter& operator<<(Emitter& out, const glm::u16vec2& in)
{
        out << Flow;
        out << BeginSeq << (in.r * (1.0f / 64.0f)) << (in.g * (1.0f / 64.0f)) << EndSeq;
        return out;
}

static Emitter& operator<<(Emitter& out, const glm::i16vec2& in)
{
        out << Flow;
        out << BeginSeq << (in.r * (1.0f / 64.0f)) << (in.g * (1.0f / 64.0f)) << EndSeq;
        return out;
}

static Emitter& operator<<(Emitter& out, const Borek::Symbol& in)
{
        out << Flow;
        out << in.Str();
        return out;
}

}  // namespace YAML

namespace Borek {

SceneSerializer::SceneSerializer(Ref<Scene> scene) : m_Scene(scene)
{
}

static void
serialize_msg(YAML::Emitter& out, MsgConnection& conenction, Entity e,
              std::string_view key)
{
        if (conenction.IsNil())
                return;

        out << YAML::Key << key << YAML::Value;
        out << YAML::convert<MsgConnection>::encode(conenction, e);
}

static void
deserialize_msg(YAML::Node node, MsgConnection* connection, Entity e,
                SceneSerializer& serializer)
{
        if (!node) return;

        mrb_state* mrb = Application::GetRubyEngine().GetRubyVM();
        connection->rbsym = mrb_intern_cstr(mrb, node["Callback"].as<std::string>().c_str());
        serializer.AddMsgConnectionCommand(
                node["TargetPath"].as<std::string_view>(), connection, e);
}

static void
serialize_id(YAML::Emitter& out, Entity e, SceneSerializer& _)
{
        auto& t= e.GetComponent<IDComponent>();
        out << YAML::Key << "ID" << YAML::BeginMap;

        out << YAML::Key << "Value" << YAML::Value << t.id.GetValue();

        out << YAML::EndMap;
}

static void
serialize_transform(YAML::Emitter& out, Entity e, SceneSerializer& _)
{
        auto& t= e.GetComponent<TransformComponent>();
        out << YAML::Key << "Transform" << YAML::BeginMap;

        out << YAML::Key << "Position" << YAML::Value << t.position;
        out << YAML::Key << "Scale" << YAML::Value << t.scale;
        out << YAML::Key << "Rotation" << YAML::Value << t.rotation;
        out << YAML::Key << "Pivot" << YAML::Value << t.pivot;

        out << YAML::EndMap;
}

static void
serialize_sprite(YAML::Emitter& out, Entity e, SceneSerializer& _)
{
        if (!e.HasComponent<SpriteComponent>())
                return;

        auto& spr = e.GetComponent<SpriteComponent>();
        out << YAML::Key << "Sprite" << YAML::BeginMap;

        out << YAML::Key << "Color" << YAML::Value << spr.color;
        out << YAML::Key << "SizeX" << YAML::Value << spr.size_x;
        out << YAML::Key << "SizeY" << YAML::Value << spr.size_y;
        if (spr.texture.IsValid())
                out << YAML::Key << "AssetPath" << YAML::Value << spr.texture.GetPath();

        out << YAML::EndMap;
}

static void
serialize_script(YAML::Emitter& out, Entity e, SceneSerializer& _)
{
        if (!e.HasComponent<RubyScriptComponent>())
                return;

        auto& scr = e.GetComponent<RubyScriptComponent>();
        out << YAML::Key << "Script" << YAML::BeginMap;

        out << YAML::Key << "ScriptPath" << YAML::Value << scr.script.GetPath();

        out << YAML::EndMap;
}

static void
serialize_camera(YAML::Emitter& out, Entity e, SceneSerializer& _)
{
        if (!e.HasComponent<CameraComponent>())
                return;

        auto& camera = e.GetComponent<CameraComponent>();
        out << YAML::Key << "Camera" << YAML::BeginMap;

        out << YAML::Key << "IsActive" << YAML::Value << camera.is_active;
        out << YAML::Key << "Width" << YAML::Value << camera.width;
        out << YAML::Key << "Height" << YAML::Value << camera.height;

        out << YAML::EndMap;
}

static void
serialize_rigid_body_2d(YAML::Emitter& out, Entity e, SceneSerializer& _)
{
        if (!e.HasComponent<BodyComponent>())
                return;

        auto& body = e.GetComponent<BodyComponent>();
        out << YAML::Key << "RigidBody2D" << YAML::BeginMap;

        out << YAML::Key << "ColliderType" << YAML::Value << magic_enum::enum_name(body.collider_type);
        out << YAML::Key << "CollisionMask" << YAML::Value << body.collision_mask;
        switch (body.collider_type) {
        case FZX::ColliderType::Rectangle:
                out << YAML::Key << "SizeX" << YAML::Value << body.rc.size_x;
                out << YAML::Key << "SizeY" << YAML::Value << body.rc.size_y;
                out << YAML::Key << "Rotation" << YAML::Value << body.rc.rotation;
                break;
        case FZX::ColliderType::Circle:
                out << YAML::Key << "Radius" << YAML::Value << body.cc.radius;
                break;
        default:
                break;
        }

        out << YAML::EndMap;
}

static void
serialize_text2d(YAML::Emitter& out, Entity e, SceneSerializer& _)
{
        if (!e.HasComponent<Text2DComponent>())
                return;

        auto& text2d = e.GetComponent<Text2DComponent>();
        out << YAML::Key << "Text2D" << YAML::BeginMap;

        out << YAML::Key << "Size" << YAML::Value << text2d.size;
        out << YAML::Key << "Value" << YAML::Value << text2d.value;
        out << YAML::Key << "Color" << YAML::Value << text2d.color;
        if (text2d.font.IsValid()) {
                out << YAML::Key << "FontPath" <<
                        YAML::Value << text2d.font.GetPath();
        }

        out << YAML::EndMap;
}

static void
serialize_tilemap(YAML::Emitter& out, Entity e, SceneSerializer& _)
{
        if (!e.HasComponent<TilemapComponent>())
                return;

        auto& tmap = e.GetComponent<TilemapComponent>();
        out << YAML::Key << "Tilemap" << YAML::BeginMap;

        out << YAML::Key << "StepX" << YAML::Value << tmap.step_x;
        out << YAML::Key << "StepY" << YAML::Value << tmap.step_y;
        if (tmap.tilemap.IsValid()) {
                out << YAML::Key << "TilemapPath" <<
                        YAML::Value << tmap.tilemap.GetPath();
        }

        out << YAML::Key << "Tiles" << YAML::Flow << YAML::BeginSeq;
        for (auto it : tmap.items) {
                out << YAML::BeginMap;

                out << YAML::Key << "Row" << YAML::Value << it.first.first;
                out << YAML::Key << "Col" << YAML::Value << it.first.second;
                out << YAML::Key << "Index" << YAML::Value << it.second;

                out << YAML::EndMap;
        }
        out << YAML::EndSeq;

        out << YAML::EndMap;
}

static void
serialize_sound_player(YAML::Emitter& out, Entity e, SceneSerializer& _)
{
        if (!e.HasComponent<SoundPlayerComponent>())
                return;

        auto& sp = e.GetComponent<SoundPlayerComponent>();
        out << YAML::Key << "SoundPlayer" << YAML::BeginMap;
        out << YAML::Key << "Flags" << YAML::Value << sp.flags.val;

        serialize_msg(out, *sp.on_finished, e, "OnFinished");

        if (sp.asset.IsValid()) {
                out << YAML::Key << "SoundPath" <<
                        YAML::Value << sp.asset.GetPath();
        }

        out << YAML::EndMap;
}

static void
serialize_prefab(YAML::Emitter& out, Entity e, SceneSerializer& _)
{
        if (!e.HasComponent<PrefabComponent>())
                return;

        auto& pf = e.GetComponent<PrefabComponent>();
        out << YAML::Key << "Prefab" << YAML::BeginMap;

        if (pf.scene.IsValid()) {
                out << YAML::Key << "ScenePath" <<
                        YAML::Value << pf.scene.GetPath();
        }

        out << YAML::EndMap;
}

static void
serialize_animated_sprite(YAML::Emitter& out, Entity e, SceneSerializer& _)
{
        if (!e.HasComponent<AnimatedSpriteComponent>())
                return;

        auto& spr = e.GetComponent<AnimatedSpriteComponent>();
        out << YAML::Key << "AnimatedSprite" << YAML::BeginMap;

        out << YAML::Key << "Color" << YAML::Value << spr.color;
        out << YAML::Key << "SizeX" << YAML::Value << spr.size_x;
        out << YAML::Key << "SizeY" << YAML::Value << spr.size_y;
        out << YAML::Key << "Flags" << YAML::Value << spr.flags;
        out << YAML::Key << "CurrentAnimation" << YAML::Value << spr.current_animation;
        if (spr.anim.IsValid())
                out << YAML::Key << "AnimPath" << YAML::Value << spr.anim.GetPath();

        out << YAML::EndMap;
}

static void
serialize_area(YAML::Emitter& out, Entity e, SceneSerializer& _)
{
        if (!e.HasComponent<AreaComponent>())
                return;

        auto& area = e.GetComponent<AreaComponent>();
        out << YAML::Key << "Area" << YAML::BeginMap;

        out << YAML::Key << "ColliderType" << YAML::Value << magic_enum::enum_name(area.collider_type);
        out << YAML::Key << "CollisionMask" << YAML::Value << area.collision_mask;
        switch (area.collider_type) {
        case FZX::ColliderType::Rectangle:
                out << YAML::Key << "SizeX" << YAML::Value << area.rc.size_x;
                out << YAML::Key << "SizeY" << YAML::Value << area.rc.size_y;
                out << YAML::Key << "Rotation" << YAML::Value << area.rc.rotation;
                break;
        case FZX::ColliderType::Circle:
                out << YAML::Key << "Radius" << YAML::Value << area.cc.radius;
                break;
        default:
                break;
        }

        serialize_msg(out, area.callbacks->on_area_entered, e, "OnAreaEntered");
        serialize_msg(out, area.callbacks->on_area_exited, e, "OnAreaExited");
        serialize_msg(out, area.callbacks->on_body_entered, e, "OnBodyEntered");
        serialize_msg(out, area.callbacks->on_body_exited, e, "OnBodyExited");

        out << YAML::EndMap;
}

static void
serialize_tags(YAML::Emitter& out, Entity e, SceneSerializer& _)
{
        if (!e.HasComponent<TagComponent>())
                return;

        auto& tags = e.GetComponent<TagComponent>();
        if (tags.empty())
                return;

        out << YAML::Key << "Tags" << YAML::Flow << YAML::BeginSeq;

        for (Symbol& sym : tags) {
                out << sym.Str();
        }

        out << YAML::EndSeq;
}

static void
serialize_values(YAML::Emitter& out, Entity e, SceneSerializer& _)
{
        if (!e.HasComponent<ValueComponent>())
                return;

        auto& values = e.GetComponent<ValueComponent>();
        if (values.empty())
                return;

        out << YAML::Key << "Values" << YAML::Flow << YAML::BeginSeq;

        for (const Value& val : values) {
                out << YAML::BeginMap << YAML::Key << "Name";
                out << YAML::Value << val.name.Str();
                out << YAML::Key << "Type" << YAML::Value;

                switch (val.type) {
                case ValueType_Bool:
                        out << "Bool" << YAML::Key << "Val" << YAML::Value;
                        out << val.boolean;
                        break;
                case ValueType_Int:
                        out << "Int" << YAML::Key << "Val" << YAML::Value;
                        out << val.integer;
                        break;
                case ValueType_String:
                        out << "String" << YAML::Key << "Val" << YAML::Value;
                        out << val.str.Str();
                        break;
                case ValueType_Number:
                        out << "Number" << YAML::Key << "Val" << YAML::Value;
                        out << val.number;
                        break;
                case ValueType_Color:
                        out << "Color" << YAML::Key << "Val" << YAML::Value;
                        out << val.color;
                        break;
                case ValueType_Node:
                        out << "Node" << YAML::Key << "Val" << YAML::Value;
                        out << val.node.path;
                        break;
                case ValueType_Asset:
                        out << "Asset" << YAML::Key << "Val" << YAML::Value;
                        out << val.asset.path;
                        break;
                }

                out << YAML::EndMap;
        }

        out << YAML::EndSeq;
}

static void
serialize_particle_emmiter(YAML::Emitter& out, Entity e, SceneSerializer& _)
{
        if (!e.HasComponent<ParticleComponent>())
                return;

        auto& p = e.GetComponent<ParticleComponent>();
        out << YAML::Key << "ParticleEmmiter" << YAML::BeginMap;

        out << YAML::Key << "SizeVariation" << YAML::Value << p.size_variation;
        out << YAML::Key << "VelocityVariation" << YAML::Value << p.velocity_variation;
        out << YAML::Key << "Velocity" << YAML::Value << p.velocity;
        out << YAML::Key << "FromColor" << YAML::Value << p.from_color;
        out << YAML::Key << "ToColor" << YAML::Value << p.to_color;
        out << YAML::Key << "ToSize" << YAML::Value << p.to_size;
        out << YAML::Key << "FromSize" << YAML::Value << p.from_size;
        out << YAML::Key << "Lifetime" << YAML::Value << p.lifetime;
        out << YAML::Key << "EmmitsPerSecond" << YAML::Value << p.emmits_per_second;
        out << YAML::Key << "Enabled" << YAML::Value << p.enabled;

        out << YAML::EndMap;
}

using serialize_fn = void(*)(YAML::Emitter&, Entity, SceneSerializer&);
using deserialize_fn = void(*)(YAML::Node&, Entity, SceneSerializer&);

static std::array<serialize_fn, 4> base_serialize_functions = {
        serialize_id, serialize_transform, serialize_script, serialize_values
};

static std::array<serialize_fn, 10> other_serialize_functions = {
        serialize_sprite, serialize_camera, serialize_rigid_body_2d,
        serialize_text2d, serialize_tilemap, serialize_sound_player,
        serialize_animated_sprite, serialize_area, serialize_tags,
        serialize_particle_emmiter,
};


static void
serialize_components(YAML::Emitter& out, Entity e, SceneSerializer& ss)
{
        out << YAML::Key << "Components" << YAML::BeginMap;

        for (auto& fn : base_serialize_functions) {
                fn(out, e, ss);
        }
        if (e.HasComponent<PrefabComponent>()) {
                serialize_prefab(out, e, ss);
        } else {
                for (auto& fn : other_serialize_functions) {
                        fn(out, e, ss);
                }
        }

        out << YAML::EndMap;
}

static void
serialize_entity(YAML::Emitter& out, Entity e, SceneSerializer& ss)
{
        out << YAML::BeginMap << YAML::Key << "Name";
        out << YAML::Value << e.GetName();
        out << YAML::Key << "Type" << YAML::Value << get_node_type_string(e.GetNodeType());
        serialize_components(out, e, ss);

        if (!e.HasComponent<PrefabComponent>() && e.HasChildren()) {
                out << YAML::Key << "Children" << YAML::BeginSeq;

                for (uint32_t id : *Application::GetScene()->GetTree().GetEntityChildren(e)) {
                        serialize_entity(out, Entity(id), ss);
                }

                out << YAML::EndSeq;
        }

        out << YAML::EndMap;
}

void SceneSerializer::Serialize(const std::filesystem::path& path, Entity e)
{
        BOREK_ASSERT(m_Scene, "Trying to serialize null ptr scene");
        YAML::Emitter out;
        out << YAML::BeginMap;
        out << YAML::Key << "Scene" << YAML::Value << path.filename();
        out << YAML::Key << "Root";

        if (e.IsNil())
                e = m_Scene->GetTree().GetRootEntity();
        serialize_entity(out, e, *this);
        out << YAML::EndMap;

        std::ofstream fout(path);
        fout << out.c_str();
}

static void
deserialize_id(YAML::Node& data, Entity e, SceneSerializer& _)
{
        auto& id = e.GetComponent<IDComponent>();
        auto iddata = data["ID"];
        if (iddata)
                id.id = UUID(iddata["Value"].as<uint64_t>());
}

static void
deserialize_transform(YAML::Node& data, Entity e, SceneSerializer& _)
{
        auto& t = e.GetComponent<TransformComponent>();
        auto tdata = data["Transform"];
        t.pivot = tdata["Pivot"].as<glm::i16vec2>();

        if (t.position != glm::vec2(0) || t.rotation != 0 || t.scale != glm::u16vec2(64))
                return;

        t.position = tdata["Position"].as<glm::vec2>();
        t.scale = tdata["Scale"].as<glm::u16vec2>();
        t.rotation = tdata["Rotation"].as<float>();
}

static void
deserialize_sprite(YAML::Node& data, Entity e, SceneSerializer& _)
{
        if (auto spr = data["Sprite"]) {
                auto& t = e.GetComponent<SpriteComponent>();

                t.color = spr["Color"].as<Color>();
                t.size_x = spr["SizeX"].as<uint16_t>();
                t.size_y = spr["SizeY"].as<uint16_t>();
                if (spr["AssetPath"])
                        t.texture = AssetManager::Get<TexAsset>(spr["AssetPath"].as<std::string>());
        }
}

static void
deserialize_camera(YAML::Node& data, Entity e, SceneSerializer& _)
{
        if (auto cam = data["Camera"]) {
                auto& c = e.GetComponent<CameraComponent>();

                auto cdata = data["Camera"];
                c.is_active = cdata["IsActive"].as<bool>();
                c.width = cdata["Width"].as<uint16_t>();
                c.height = cdata["Height"].as<uint16_t>();

                //BOREK_LOG_INFO("Implement camera deserialization");
        }
}

static void
deserialize_script(YAML::Node& data, Entity e, SceneSerializer& _)
{
        if (auto cam = data["Script"]) {
                if (!e.HasComponent<RubyScriptComponent>()) {
                        e.AddComponent<RubyScriptComponent>();
                }

                auto& t = e.GetComponent<RubyScriptComponent>();
                t.script = AssetManager::Get<ScriptAsset>(data["Script"]["ScriptPath"].as<std::string>());
        }
}

static void
deserialize_rigid_body_2d(YAML::Node& data, Entity e, SceneSerializer& _)
{
        if (auto rbdata = data["RigidBody2D"]) {
                auto& body = e.GetComponent<BodyComponent>();

                body.collision_mask = rbdata["CollisionMask"].as<uint16_t>();
                body.collider_type = magic_enum::enum_cast<FZX::ColliderType>(
                        rbdata["ColliderType"].as<std::string>()).value_or(FZX::ColliderType::Rectangle);

                switch (body.collider_type) {
                case FZX::ColliderType::Rectangle:
                        body.rc.size_x = rbdata["SizeX"].as<uint16_t>();
                        body.rc.size_y = rbdata["SizeY"].as<uint16_t>();
                        body.rc.rotation = rbdata["Rotation"].as<float>();
                        break;
                case FZX::ColliderType::Circle:
                        body.cc.radius = rbdata["Radius"].as<uint16_t>();
                        break;
                default:
                        break;
                }
        }
}

static void
deserialize_area(YAML::Node& data, Entity e, SceneSerializer& ss)
{
        if (auto aread = data["Area"]) {
                auto& area = e.GetComponent<AreaComponent>();

                area.collision_mask = aread["CollisionMask"].as<uint16_t>();
                area.collider_type = magic_enum::enum_cast<FZX::ColliderType>(
                        aread["ColliderType"].as<std::string>()).value_or(FZX::ColliderType::Rectangle);

                switch (area.collider_type) {
                case FZX::ColliderType::Rectangle:
                        area.rc.size_x = aread["SizeX"].as<uint16_t>();
                        area.rc.size_y = aread["SizeY"].as<uint16_t>();
                        area.rc.rotation = aread["Rotation"].as<float>();
                        break;
                case FZX::ColliderType::Circle:
                        area.cc.radius = aread["Radius"].as<uint16_t>();
                        break;
                default:
                        break;
                }

                deserialize_msg(aread["OnAreaEntered"],
                                &area.callbacks->on_area_entered, e, ss);
                deserialize_msg(aread["OnAreaExited"],
                                &area.callbacks->on_area_exited, e, ss);
                deserialize_msg(aread["OnBodyEntered"],
                                &area.callbacks->on_body_entered, e, ss);
                deserialize_msg(aread["OnBodyExited"],
                                &area.callbacks->on_body_exited, e, ss);
        }
}

static void
deserialize_text2d(YAML::Node& data, Entity e, SceneSerializer& _)
{
        if (auto textd = data["Text2D"]) {
                auto& text2d = e.GetComponent<Text2DComponent>();

                text2d.value = textd["Value"].as<std::string>();
                text2d.size = textd["Size"].as<float>();
                text2d.color = textd["Color"].as<Color>();
                if (auto asset = textd["FontPath"]) {
                        text2d.font = AssetManager::Get<FontAsset>(asset.as<std::string>());
                }
        }
}

static void
deserialize_tilemap(YAML::Node& data, Entity e, SceneSerializer& _)
{
        if (auto tmapd = data["Tilemap"]) {
                auto& tmap = e.GetComponent<TilemapComponent>();

                tmap.step_x = tmapd["StepX"].as<uint16_t>();
                tmap.step_y = tmapd["StepY"].as<uint16_t>();
                if (auto asset = tmapd["TilemapPath"]) {
                        tmap.tilemap = AssetManager::Get<TilemapAsset>(asset.as<std::string>());
                }

                for (auto item : tmapd["Tiles"]) {
                        const short row = item["Row"].as<short>();
                        const short col = item["Col"].as<short>();
                        const short index = item["Index"].as<short>();

                        tmap.Set(col, row, index);
                }
        }
}

static void
deserialize_sound_player(YAML::Node& data, Entity e, SceneSerializer& ss)
{
        if (auto spd = data["SoundPlayer"]) {
                auto& sp = e.GetComponent<SoundPlayerComponent>();

                sp.flags.val = spd["Flags"].as<uint32_t>();
                if (spd["SoundPath"]) {
                        sp.asset = AssetManager::Get<SoundAsset>(spd["SoundPath"].as<std::string>());
                }

                deserialize_msg(spd["OnFinished"], sp.on_finished.get(), e, ss);
        }
}

static void
deserialize_animated_sprite(YAML::Node& data, Entity e, SceneSerializer& _)
{
        if (auto sprd = data["AnimatedSprite"]) {
                auto& spr = e.GetComponent<AnimatedSpriteComponent>();

                spr.color = sprd["Color"].as<Color>();
                spr.size_x = sprd["SizeX"].as<uint16_t>();
                spr.size_y = sprd["SizeY"].as<uint16_t>();
                spr.flags = sprd["Flags"].as<uint32_t>();
                spr.current_animation = sprd["CurrentAnimation"].as<Symbol>();
                if (sprd["AnimPath"])
                        spr.anim = AssetManager::Get<AnimationAsset>(sprd["AnimPath"].as<std::string>());

                if (spr.anim.IsValid() && !spr.current_animation.IsNil()) {
                        spr.flags.SetFlags(AnimatedSpriteComponentFlags_Playing, true);
                        spr.current_frame = spr.anim->animation_by_name[spr.current_animation] + 1;
                }
        }
}

static void
deserialize_prefab(YAML::Node& data, Entity e, SceneSerializer& _)
{
        auto pfd = data["Prefab"];
        auto& pf = e.AddComponent<PrefabComponent>()
                    .GetComponent<PrefabComponent>();

        if (pfd["ScenePath"])
                pf.scene = AssetManager::Get<SceneAsset>(pfd["ScenePath"].as<std::string>());

        pf.Update(e);
}

static void
deserialize_tags(YAML::Node& data, Entity e, SceneSerializer& _)
{
        if (auto tagd = data["Tags"]) {
                auto& tag = e.GetComponent<TagComponent>();

                for (auto item : tagd) {
                        tag.AddTag(item.as<std::string>());
                }
        }
}

static void
deserialize_values(YAML::Node& data, Entity e, SceneSerializer& ss)
{
        if (auto valsd = data["Values"]) {
                if (!e.HasComponent<ValueComponent>())
                        e.AddComponent<ValueComponent>();

                auto& vals = e.GetComponent<ValueComponent>();

                for (auto val : valsd) {
                        Symbol name = val["Name"].as<std::string_view>();
                        if (ss.IsSerializingPrefab() && vals.contains(Value(name, ValueType_Bool))) {
                                BOREK_LOG_ERROR("Dropped {}", name.Str());
                                continue;
                        }
                        else if (e.HasComponent<PrefabComponent>() && !ss.IsSerializingPrefab() && !vals.contains(Value(name, ValueType_Bool))) {
                                BOREK_LOG_WARN("Dropped {}", name.Str());
                                continue;
                        }

                        switch (Hash(val["Type"].as<std::string>())) {
                        case Hash("Bool"):
                                vals.Add(name, val["Val"].as<bool>());
                                break;
                        case Hash("String"):
                                vals.Add(name, val["Val"].as<std::string>());
                                break;
                        case Hash("Number"):
                                vals.Add(name, val["Val"].as<double>());
                                break;
                        case Hash("Int"):
                                vals.Add(name, val["Val"].as<int64_t>());
                                break;
                        case Hash("Color"):
                                vals.Add(name, ValueType_Color);
                                vals.Get(name)->color = val["Val"].as<Color>();
                                break;
                        case Hash("Node"):
                                vals.Add(name, ValueType_Node);
                                vals.Get(name)->node.path = val["Val"].as<Symbol>();
                                break;
                        case Hash("Asset"):
                                vals.Add(name, ValueType_Asset);
                                vals.Get(name)->asset.path = val["Val"].as<Symbol>();
                                break;
                        }
                }
        }
}

static void
deserialize_particle_emmiter(YAML::Node& data, Entity e, SceneSerializer& _)
{
        auto ped = data["ParticleEmmiter"];
        if (!ped)
                return;

        auto& p = e.GetComponent<ParticleComponent>();

        p.size_variation = ped["SizeVariation"].as<float>();
        p.velocity_variation = ped["VelocityVariation"].as<glm::vec2>();
        p.velocity = ped["Velocity"].as<glm::vec2>();
        p.from_color = ped["FromColor"].as<Color>();
        p.to_color = ped["ToColor"].as<Color>();
        p.to_size = ped["ToSize"].as<float>();
        p.from_size = ped["FromSize"].as<float>();
        p.lifetime = ped["Lifetime"].as<float>();
        p.emmits_per_second = ped["EmmitsPerSecond"].as<uint32_t>();
        p.enabled = ped["Enabled"].as<bool>();
}

static std::array<deserialize_fn, 11> deserialize_functions = {
        deserialize_sprite, deserialize_camera, deserialize_rigid_body_2d,
        deserialize_text2d, deserialize_tilemap, deserialize_sound_player,
        deserialize_animated_sprite, deserialize_area, deserialize_tags,
        deserialize_particle_emmiter, deserialize_script,
};

static std::array<deserialize_fn, 3> base_deserialize_functions = {
        deserialize_id, deserialize_transform, deserialize_values,
};

static Entity
deserialize_entity(YAML::Node ynode, Scene& scene, SceneSerializer& ss,
                   Entity e = Entity())
{
        if (e.IsNil()) {
                e = scene.NewEntity(ynode["Name"].as<std::string>(),
                                    get_node_type(ynode["Type"].as<std::string>()));
        } else {
                e.InitializeNode(get_node_type(ynode["Type"].as<std::string>()));
                e.GetComponent<IDComponent>().id = UUID();
                e.DeleteChildren();
        }

        auto components = ynode["Components"];
        if (components["Prefab"]) {
                deserialize_prefab(components, e, ss);
        } else {
                for (auto& fn : deserialize_functions) {
                        fn(components, e, ss);
                }
        }

        for (auto& fn : base_deserialize_functions) {
                fn(components, e, ss);
        }

        auto children = ynode["Children"];
        if (children) {
                for (auto child : children) {
                        Entity c = deserialize_entity(child, scene, ss);
                        scene.GetTree().EntityAppendChild(c, e);
                };
        }

        return e;
}

void
SceneSerializer::Deserialize(const std::filesystem::path& path, Entity e)
{
        YAML::Node data = YAML::LoadFile(path);
        BOREK_ENGINE_ASSERT(data["Scene"], "Trying to deserialize invalid scene");

        std::string scene_name = data["Scene"].as<std::string>();

        SetSerializingPrefab(!e.IsNil());
        Entity res = deserialize_entity(data["Root"], *m_Scene, *this, e);
        if (e.IsNil()) {
                m_Scene->SetPath(path);
                m_Scene->GetTree().SetRootEntity(res);
        }

        HandleCommands();
}

void
SceneSerializer::HandleCommands()
{
        constexpr auto cmd_visitor = overloads {
                [](InitMsgConnection& cmd){
                        Entity from(cmd.entity);
                        cmd.connection->entity_id = from.FindEntityByPath(cmd.path);
                }
        };

        for (auto& cmd : m_Commands) {
                std::visit(cmd_visitor, cmd.value);
        }
}

bool
SceneSerializer::IsSerializingPrefab()
{
        return m_SerializingPrefab;
}

void
SceneSerializer::SetSerializingPrefab(bool val)
{
        m_SerializingPrefab = val;
}


void
SceneSerializer::AddMsgConnectionCommand(std::string_view path,
                                         struct MsgConnection* connection,
                                         uint32_t entity)
{
        m_Commands.emplace_back(InitMsgConnection{
                std::string(path), connection, entity});
}

}  // namespace Borek
