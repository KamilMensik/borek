// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Base/Application.h"
#include "Include/Base/Entity.h"
#include "Include/Base/Node.h"
#include "Include/Components/PrefabComponent.h"
#include "Include/Components/SoundplayerComponent.h"
#include "Include/Components/TilemapComponent.h"
#include "Include/Engine/Assets/Asset.h"
#include "Include/Engine/Assets/ScriptAsset.h"
#include "Include/Engine/Assets/SoundAsset.h"
#include "Include/Engine/Assets/TilemapAsset.h"
#include "Include/Engine/Assets/SceneAsset.h"
#include <cstdint>
#include <fstream>
#include <memory>

#include <yaml-cpp/yaml.h>
#include <yaml-cpp/emitter.h>
#include <yaml-cpp/emittermanip.h>
#include <ECS/World.h>
#include <magic_enum/magic_enum.hpp>

#include "Include/Engine/SceneSerializer.h"
#include "Include/Base/UUID.h"
#include "Include/Graphics/Camera.h"
#include "Include/Components/TransformComponent.h"
#include "Include/Components/IDComponent.h"
#include "Include/Components/TagComponent.h"
#include "Include/Components/Text2DComponent.h"
#include "Include/Components/SpriteComponent.h"
#include "Include/Components/CameraComponent.h"
#include "Include/Components/RubyScriptComponent.h"

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

static Emitter& operator<<(Emitter& out, glm::vec3& in)
{
        out << Flow;
        out << BeginSeq << in.x << in.y << in.z << EndSeq;
        return out;
}

static Emitter& operator<<(Emitter& out, glm::vec4& in)
{
        out << Flow;
        out << BeginSeq << in.r << in.g << in.b << in.a << EndSeq;
        return out;
}

static Emitter& operator<<(Emitter& out, glm::vec2& in)
{
        out << Flow;
        out << BeginSeq << in.r << in.g << EndSeq;
        return out;
}

}  // namespace YAML

namespace Borek {

SceneSerializer::SceneSerializer(Ref<Scene> scene) : m_Scene(scene)
{
}

void SerializeID(YAML::Emitter& out, Entity e)
{
        auto& t= e.GetComponent<IDComponent>();
        out << YAML::Key << "ID" << YAML::BeginMap;

        out << YAML::Key << "Value" << YAML::Value << t.id.GetValue();

        out << YAML::EndMap;
}

void SerializeTransform(YAML::Emitter& out, Entity e)
{
        auto& t= e.GetComponent<TransformComponent>();
        out << YAML::Key << "Transform" << YAML::BeginMap;

        out << YAML::Key << "Position" << YAML::Value << t.position;
        out << YAML::Key << "Scale" << YAML::Value << t.scale;
        out << YAML::Key << "Rotation" << YAML::Value << t.rotation;

        out << YAML::EndMap;
}

void SerializeSprite(YAML::Emitter& out, Entity e)
{
        if (!e.HasComponent<SpriteComponent>())
                return;

        auto& spr = e.GetComponent<SpriteComponent>();
        out << YAML::Key << "Sprite" << YAML::BeginMap;

        out << YAML::Key << "Color" << YAML::Value << spr.color;
        if (spr.texture.IsValid())
                out << YAML::Key << "AssetPath" << YAML::Value << spr.texture.GetPath();

        out << YAML::EndMap;
}

void SerializeScript(YAML::Emitter& out, Entity e)
{
        if (!e.HasComponent<RubyScriptComponent>())
                return;

        auto& scr = e.GetComponent<RubyScriptComponent>();
        out << YAML::Key << "Script" << YAML::BeginMap;

        out << YAML::Key << "ScriptPath" << YAML::Value << scr.script.GetPath();

        out << YAML::EndMap;
}

void SerializeCamera(YAML::Emitter& out, Entity e)
{
        if (!e.HasComponent<CameraComponent>())
                return;

        auto& camera = e.GetComponent<CameraComponent>();
        out << YAML::Key << "Camera" << YAML::BeginMap;

        out << YAML::Key << "IsActive" << YAML::Value << camera.is_active;

        out << YAML::EndMap;
}

void SerializeRigidBody2D(YAML::Emitter& out, Entity e)
{
        if (!e.HasComponent<FZX::BodyComponent>())
                return;

        auto& body = e.GetComponent<FZX::BodyComponent>();
        out << YAML::Key << "RigidBody2D" << YAML::BeginMap;

        out << YAML::Key << "BodyType" << YAML::Value << magic_enum::enum_name(body.body_type);
        out << YAML::Key << "ColliderType" << YAML::Value << magic_enum::enum_name(body.collider_type);
        out << YAML::Key << "CollisionMask" << YAML::Value << body.collision_mask;

        out << YAML::EndMap;
}

void SerializeText2D(YAML::Emitter& out, Entity e)
{
        if (!e.HasComponent<Text2DComponent>())
                return;

        auto& text2d = e.GetComponent<Text2DComponent>();
        out << YAML::Key << "Text2D" << YAML::BeginMap;

        out << YAML::Key << "Size" << YAML::Value << text2d.size;
        out << YAML::Key << "Value" << YAML::Value << text2d.value.c_str();
        out << YAML::Key << "Color" << YAML::Value << text2d.color;

        out << YAML::EndMap;
}

void SerializeTilemap(YAML::Emitter& out, Entity e)
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

        out << YAML::Key << "Tiles" << YAML::BeginSeq;
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

void SerializeSoundPlayer(YAML::Emitter& out, Entity e)
{
        if (!e.HasComponent<SoundPlayerComponent>())
                return;

        auto& sp = e.GetComponent<SoundPlayerComponent>();
        out << YAML::Key << "SoundPlayer" << YAML::BeginMap;
        out << YAML::Key << "Flags" << YAML::Value << sp.flags.val;

        if (sp.sound.IsValid()) {
                out << YAML::Key << "SoundPath" <<
                        YAML::Value << sp.sound.GetPath();
        }

        out << YAML::EndMap;
}

void SerializePrefab(YAML::Emitter& out, Entity e)
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

void SerializeComponents(YAML::Emitter& out, Entity e)
{
        out << YAML::Key << "Components" << YAML::BeginMap;

        SerializeID(out, e);
        SerializeTransform(out, e);
        SerializeSprite(out, e);
        SerializeScript(out, e);
        SerializeCamera(out, e);
        SerializeRigidBody2D(out, e);
        SerializeText2D(out, e);
        SerializeTilemap(out, e);
        SerializeSoundPlayer(out, e);
        SerializePrefab(out, e);

        out << YAML::EndMap;
}
template<class... Ts>
struct overloads : Ts... { using Ts::operator()...; };

void SerializeEntity(YAML::Emitter& out, Entity e)
{
        out << YAML::BeginMap << YAML::Key << "Name";
        out << YAML::Value << e.GetComponent<TagComponent>().value.c_str();
        out << YAML::Key << "Type" << YAML::Value << get_node_type_string(e.GetNodeType());
        SerializeComponents(out, e);

        if (!e.HasComponent<PrefabComponent>() && e.HasChildren()) {
                out << YAML::Key << "Children" << YAML::BeginSeq;

                for (uint32_t id : *Application::GetScene()->GetEntityChildren(e)) {
                        SerializeEntity(out, Entity(id));
                }

                out << YAML::EndSeq;
        }

        out << YAML::EndMap;
}

void SceneSerializer::Serialize(const std::string& path, Entity e)
{
        BOREK_ASSERT(m_Scene, "Trying to serialize null ptr scene");
        YAML::Emitter out;
        out << YAML::BeginMap;
        out << YAML::Key << "Scene" << YAML::Value << "Jmeno";
        out << YAML::Key << "Root";

        if (e.IsNil())
                e = m_Scene->RootEntity();
        SerializeEntity(out, e);
        out << YAML::EndMap;

        std::ofstream fout(path);
        fout << out.c_str();
}

void DeserializeID(YAML::Node& data, Entity e)
{
        auto& id = e.GetComponent<IDComponent>();
        auto iddata = data["ID"];
        if (iddata)
                id.id = UUID(iddata["Value"].as<uint64_t>());
}

void DeserializeTransform(YAML::Node& data, Entity e)
{
        auto& t = e.GetComponent<TransformComponent>();
        auto tdata = data["Transform"];
        t.position = tdata["Position"].as<glm::vec2>();
        t.scale = tdata["Scale"].as<glm::vec2>();
        t.rotation = tdata["Rotation"].as<float>();
}

void DeserializeSprite(YAML::Node& data, Entity e)
{
        if (auto spr = data["Sprite"]) {
                auto& t = e.GetComponent<SpriteComponent>();

                t.color = spr["Color"].as<glm::vec4>();
                if (spr["AssetPath"])
                        t.texture = AssetManager::Get<TexAsset>(spr["AssetPath"].as<std::string>());
        }
}

void DeserializeCamera(YAML::Node& data, Entity e)
{
        if (auto cam = data["Camera"]) {
                auto& c = e.GetComponent<CameraComponent>();

                auto cdata = data["Camera"];
                c.is_active = cdata["IsActive"].as<bool>();

                //BOREK_LOG_INFO("Implement camera deserialization");
        }
}

void DeserializeScript(YAML::Node& data, Entity e)
{
        if (auto cam = data["Script"]) {
                if (!e.HasComponent<RubyScriptComponent>()) {
                        e.AddComponent<RubyScriptComponent>();
                }

                auto& t = e.GetComponent<RubyScriptComponent>();
                t.script = AssetManager::Get<ScriptAsset>(data["Script"]["ScriptPath"].as<std::string>());
        }
}

void DeserializeRigidBody2D(YAML::Node& data, Entity e)
{
        if (auto cam = data["RigidBody2D"]) {
                auto& body = e.GetComponent<FZX::BodyComponent>();

                auto rbdata = data["RigidBody2D"];
                body.collision_mask = rbdata["CollisionMask"].as<uint16_t>();
                body.body_type = magic_enum::enum_cast<FZX::BodyType>(
                        rbdata["BodyType"].as<std::string>()).value_or(FZX::BodyType::Static);
                body.collider_type = magic_enum::enum_cast<FZX::ColliderType>(
                        rbdata["ColliderType"].as<std::string>()).value_or(FZX::ColliderType::Rectangle);
        }
}

void DeserializeText2D(YAML::Node& data, Entity e)
{
        if (auto textd = data["Text2D"]) {
                auto& text2d = e.GetComponent<Text2DComponent>();

                text2d.value = textd["Value"].as<std::string>();
                text2d.size = textd["Size"].as<float>();
                text2d.color = textd["Color"].as<glm::vec4>();
                text2d.font = nullptr;
        }
}

void DeserializeTilemap(YAML::Node& data, Entity e)
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

void DeserializeSoundPlayer(YAML::Node& data, Entity e)
{
        if (auto spd = data["SoundPlayer"]) {
                auto& sp = e.GetComponent<SoundPlayerComponent>();

                sp.flags.val = spd["Flags"].as<uint32_t>();
                if (spd["SoundPath"])
                        sp.sound = AssetManager::Get<SoundAsset>(spd["SoundPath"].as<std::string>());
        }
}

void DeserializePrefab(YAML::Node& data, Entity e)
{
        if (auto pfd = data["Prefab"]) {
                auto& pf = e.AddComponent<PrefabComponent>()
                            .GetComponent<PrefabComponent>();

                if (pfd["ScenePath"])
                        pf.scene = AssetManager::Get<SceneAsset>(pfd["ScenePath"].as<std::string>());

                pf.Update(e);
        }
}

Entity DeserializeEntity(YAML::Node ynode, Scene& scene, Entity e = Entity())
{
        if (e.IsNil()) {
                e = scene.NewEntity(ynode["Name"].as<std::string>(),
                                    get_node_type(ynode["Type"].as<std::string>()));
        } else {
                e.DeleteChildren();
        }

        auto components = ynode["Components"];
        DeserializePrefab(components, e);
        DeserializeTransform(components, e);
        DeserializeSprite(components, e);
        DeserializeCamera(components, e);
        DeserializeScript(components, e);
        DeserializeRigidBody2D(components, e);
        DeserializeText2D(components, e);
        DeserializeTilemap(components, e);
        DeserializeSoundPlayer(components, e);
        auto children = ynode["Children"];
        if (children) {
                for (auto child : children) {
                        Entity c = DeserializeEntity(child, scene);
                        scene.EntityAppendChild(c, e);
                };
        }


        return e;
}

void SceneSerializer::Deserialize(const std::string& path, Entity e)
{
        YAML::Node data = YAML::LoadFile(path);
        BOREK_ENGINE_ASSERT(data["Scene"], "Trying to deserialize invalid scene");

        std::string scene_name = data["Scene"].as<std::string>();

        Entity res = DeserializeEntity(data["Root"], *m_Scene, e);
        if (e.IsNil()) {
                m_Scene->RootEntity() = res;
        }
}

}  // namespace Borek
