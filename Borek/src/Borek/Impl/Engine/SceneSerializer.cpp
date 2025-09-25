// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Base/Application.h"
#include "Include/Base/Node.h"
#include <fstream>
#include <memory>
#include <variant>

#include <yaml-cpp/yaml.h>
#include <yaml-cpp/emitter.h>
#include <yaml-cpp/emittermanip.h>
#include <ECS/World.h>
#include <magic_enum/magic_enum.hpp>

#include "Include/Base/Components.h"
#include "Include/Engine/SceneSerializer.h"
#include "Include/Debug/Log.h"
#include "Include/Base/UUID.h"
#include "Include/Engine/Assets/AssetManager.h"
#include "Include/Graphics/Camera.h"

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
        if (spr.sprite)
                out << YAML::Key << "AssetID" << YAML::Value << spr.sprite->GetAsset().Id();

        out << YAML::EndMap;
}

void SerializeScript(YAML::Emitter& out, Entity e)
{
        if (!e.HasComponent<RubyScriptComponent>())
                return;

        auto& scr = e.GetComponent<RubyScriptComponent>();
        out << YAML::Key << "Script" << YAML::BeginMap;

        out << YAML::Key << "ClassName" << YAML::Value << scr.ruby_class;

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

        if (e.HasChildren()) {
                out << YAML::Key << "Children" << YAML::BeginSeq;

                for (uint32_t id : *Application::GetScene()->GetEntityChildren(e)) {
                        SerializeEntity(out, Entity(id));
                }

                out << YAML::EndSeq;
        }

        out << YAML::EndMap;
}

void SceneSerializer::Serialize(const std::string& path)
{
        BOREK_ASSERT(m_Scene, "Trying to serialize null ptr scene");
        YAML::Emitter out;
        out << YAML::BeginMap;
        out << YAML::Key << "Scene" << YAML::Value << "Jmeno";
        out << YAML::Key << "Root";
        SerializeEntity(out, m_Scene->RootEntity());
        out << YAML::Key << "Assets" << YAML::BeginSeq;
        for (unsigned i = 0; auto& asset : AssetManager::s_Assets) {
                out << YAML::BeginMap << YAML::Key << "Type" << YAML::Value;
                std::visit(overloads {
                        [&out](RubyScript&){ out << "Script"; },
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
                if (spr["AssetID"])
                        t.sprite = NewRef<Sprite>(spr["AssetID"].as<unsigned>());
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
                auto& t = e.AddComponent<RubyScriptComponent>()
                           .GetComponent<RubyScriptComponent>();

                t.ruby_class = data["Script"]["ClassName"].as<std::string>();
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

Entity DeserializeEntity(YAML::Node ynode, Scene& scene)
{
        Entity e = scene.NewEntity(ynode["Name"].as<std::string>(),
                                   get_node_type(ynode["Type"].as<std::string>()));
        auto components = ynode["Components"];
        DeserializeTransform(components, e);
        DeserializeSprite(components, e);
        DeserializeCamera(components, e);
        DeserializeScript(components, e);
        DeserializeRigidBody2D(components, e);
        DeserializeText2D(components, e);

        auto children = ynode["Children"];
        if (children) {
                for (auto child : children) {
                        scene.EntityAppendChild(DeserializeEntity(child, scene), e);
                };
        }

        return e;
}

void SceneSerializer::Deserialize(const std::string& path)
{
        AssetManager::Reset();

        YAML::Node data = YAML::LoadFile(path);
        BOREK_ENGINE_ASSERT(data["Scene"], "Trying to deserialize invalid scene");

        std::string scene_name = data["Scene"].as<std::string>();
        auto assets = data["Assets"];
        if (assets) {
                for (auto asset : assets) {
                        if (asset["Type"].as<std::string>() == "Texture2D") {
                                AssetManager::GetTexture(asset["Path"].as<std::string>());
                        } else if (asset["Type"].as<std::string>() == "Script") {
                                std::filesystem::path path = asset["Path"].as<std::string>();
                                std::filesystem::path real_path = std::filesystem::path(Utils::Settings::Instance().current_project_path) / path;
                                std::filesystem::path asset_path = real_path;
                                asset_path.replace_extension("scr");
                                AssetManager::RefreshScript(real_path, asset_path);
                        }
                }
        }

        m_Scene->RootEntity() = DeserializeEntity(data["Root"], *m_Scene);
}

}  // namespace Borek
