// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Engine/AssetManager.h"
#include "Include/Graphics/Camera.h"
#include "magic_enum/magic_enum.hpp"
#include <fstream>
#include <memory>
#include <variant>

#include <sstream>
#include <yaml-cpp/yaml.h>
#include <yaml-cpp/emitter.h>
#include <yaml-cpp/emittermanip.h>

#include "Include/Base/Components.h"
#include "Include/Engine/SceneSerializer.h"
#include "Include/Debug/Log.h"

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
        m_SceneQuery = m_Scene->Query(TagComponent::Id());
}

SceneSerializer::SceneSerializer() {}

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
        if (spr.sprite)
                out << YAML::Key << "AssetID" << YAML::Value << spr.sprite->GetAsset().Id();

        out << YAML::EndMap;
}

void SerializeScript(YAML::Emitter& out, Entity e)
{
        if (!e.HasComponent<ScriptComponent>())
                return;

        //auto& scr = e.GetComponent<ScriptComponent>();
        out << YAML::Key << "Script" << YAML::BeginMap;

        out << YAML::Key << "Has script" << YAML::Value << "Yes";

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
        if (!e.HasComponent<RigidBody2DComponent>())
                return;

        auto& rb2d = e.GetComponent<RigidBody2DComponent>();
        out << YAML::Key << "RigidBody2D" << YAML::BeginMap;

        out << YAML::Key << "FixedRotation" << YAML::Value << rb2d.fixed_rotation;
        out << YAML::Key << "Type" << YAML::Value << magic_enum::enum_name(rb2d.type);

        out << YAML::EndMap;
}

void SerializeBoxCollider2D(YAML::Emitter& out, Entity e)
{
        if (!e.HasComponent<BoxCollider2DComponent>())
                return;

        auto& bc2d = e.GetComponent<BoxCollider2DComponent>();
        out << YAML::Key << "BoxCollider2D" << YAML::BeginMap;

        out << YAML::Key << "Restitution" << YAML::Value << bc2d.restitution;
        out << YAML::Key << "RestitutionTreshold" << YAML::Value << bc2d.restitution_treshold;
        out << YAML::Key << "Friction" << YAML::Value << bc2d.friction;
        out << YAML::Key << "Density" << YAML::Value << bc2d.density;
        out << YAML::Key << "Offset" << YAML::Value << bc2d.offset;
        out << YAML::Key << "Size" << YAML::Value << bc2d.size;

        out << YAML::EndMap;
}

void SerializeComponents(YAML::Emitter& out, Entity e)
{
        out << YAML::Key << "Components" << YAML::BeginMap;

        SerializeTransform(out, e);
        SerializeSprite(out, e);
        SerializeScript(out, e);
        SerializeCamera(out, e);
        SerializeRigidBody2D(out, e);
        SerializeBoxCollider2D(out, e);

        out << YAML::EndMap;
}
template<class... Ts>
struct overloads : Ts... { using Ts::operator()...; };

void SceneSerializer::Serialize(const std::string& path)
{
        BOREK_ASSERT(m_Scene, "Trying to serialize null ptr scene");
        YAML::Emitter out;
        out << YAML::BeginMap;
        out << YAML::Key << "Scene" << YAML::Value << "Jmeno";
        out << YAML::Key << "Entities" << YAML::BeginSeq;
        m_SceneQuery.each([&out, this](ECS::View& view){
                Entity e = Entity(view.current_entity_id, m_Scene.get());
                out << YAML::BeginMap << YAML::Key << "Name";
                out << YAML::Value << view.get<TagComponent>().value.c_str();

                SerializeComponents(out, e);

                out << YAML::EndMap;
        });
        out << YAML::EndSeq;
        out << YAML::Key << "Assets" << YAML::BeginSeq;
        for (unsigned i = 0; auto& asset : AssetManager::s_Assets) {
                out << YAML::BeginMap << YAML::Key << "Type" << YAML::Value;
                std::visit(overloads {
                        [&out](Ref<Graphics::Texture2D>&){ out << "Texture2D"; },
                }, asset);
                out << YAML::Key << "Path" << AssetManager::GetPath(i);
                out << YAML::EndMap;
                i++;
        }

        out << YAML::EndSeq;
        out << YAML::EndMap;

        std::ofstream fout(path);
        fout << out.c_str();
}


void DeserializeTransform(YAML::Node data, Entity e)
{
        auto& t = e.GetComponent<TransformComponent>();
        auto tdata = data["Transform"];
        t.position = tdata["Position"].as<glm::vec3>();
        t.scale = tdata["Scale"].as<glm::vec3>();
        t.rotation = tdata["Rotation"].as<glm::vec3>();
}

void DeserializeSprite(YAML::Node data, Entity e)
{
        if (auto spr = data["Sprite"]) {
                auto& t = e.AddComponent<SpriteComponent>()
                           .GetComponent<SpriteComponent>();

                t.color = spr["Color"].as<glm::vec4>();
                if (spr["AssetID"])
                        t.sprite = NewRef<Sprite>(spr["AssetID"].as<unsigned>());
        }
}

void DeserializeCamera(YAML::Node data, Entity e)
{
        if (auto cam = data["Camera"]) {
                auto& c = e.AddComponent<CameraComponent>()
                           .GetComponent<CameraComponent>();

                auto cdata = data["Camera"];
                c.is_active = cdata["IsActive"].as<bool>();

                BOREK_LOG_INFO("Implement camera deserialization");
        }
}

void DeserializeScript(YAML::Node data, Entity e)
{
        if (auto cam = data["Script"]) {
                //auto& t = e.AddComponent<ScriptComponent>()
                           //.GetComponent<ScriptComponent>();

                BOREK_LOG_INFO("Implement script deserialization");
        }
}

void DeserializeRigidBody2D(YAML::Node data, Entity e)
{
        if (auto cam = data["RigidBody2D"]) {
                auto& rb2d = e.AddComponent<RigidBody2DComponent>()
                              .GetComponent<RigidBody2DComponent>();

                auto rbdata = data["RigidBody2D"];
                rb2d.fixed_rotation = rbdata["FixedRotation"].as<bool>();
                rb2d.type = magic_enum::enum_cast<RigidBody2DComponent::Type>(
                        rbdata["Type"].as<std::string>()).value_or(RigidBody2DComponent::Type::kStatic);
        }
}

void DeserializeBoxCollider2D(YAML::Node data, Entity e)
{
        if (auto cam = data["BoxCollider2D"]) {
                auto& rb2d = e.AddComponent<BoxCollider2DComponent>()
                              .GetComponent<BoxCollider2DComponent>();

                auto bcdata = data["BoxCollider2D"];
                rb2d.density = bcdata["Density"].as<float>();
                rb2d.friction = bcdata["Friction"].as<float>();
                rb2d.restitution = bcdata["Restitution"].as<float>();
                rb2d.restitution_treshold = bcdata["RestitutionTreshold"].as<float>();
                rb2d.size = bcdata["Size"].as<glm::vec2>();
                rb2d.offset = bcdata["Offset"].as<glm::vec2>();
        }
}

Ref<Scene> SceneSerializer::Deserialize(const std::string& path)
{
        auto scene = std::make_shared<Scene>();
        AssetManager::Reset();

        YAML::Node data = YAML::LoadFile(path);
        BOREK_ENGINE_ASSERT(data["Scene"], "Trying to deserialize invalid scene");

        std::string scene_name = data["Scene"].as<std::string>();
        auto assets = data["Assets"];
        if (assets) {
                for (auto asset : assets) {
                        if (asset["Type"].as<std::string>() == "Texture2D") {
                                AssetManager::GetTexture(asset["Path"].as<std::string>());
                        }
                }
        }
        auto entities = data["Entities"];
        if (entities) {
                for (auto entity : entities) {
                        Entity e = scene->NewEntity(entity["Name"].as<std::string>());
                        auto components = entity["Components"];
                        DeserializeTransform(components, e);
                        DeserializeSprite(components, e);
                        DeserializeCamera(components, e);
                        DeserializeScript(components, e);
                        DeserializeRigidBody2D(components, e);
                        DeserializeBoxCollider2D(components, e);
                };
        }



        return scene;
}

}  // namespace Borek
