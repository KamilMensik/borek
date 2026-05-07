// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Components/TilemapComponent.h"
#include <cstdlib>
#include <fstream>
#include <filesystem>
#include <iostream>

#include <mrbcpp.h>
#include <glm/ext/vector_float2.hpp>
#include <yaml-cpp/yaml.h>

#include <Borek.h>
#include <Include/Core.h>
#include <Include/Base/Colors.h>
#include <Borek/Include/Engine/SceneSerializer.h>
#include <Borek/Include/Engine/Utils/Settings.h>
#include <Borek/Include/Drawing/Quad.h>
#include <Borek/Include/Base/Application.h>
#include <Borek/Include/Base/Project.h>
#include <Borek/Include/Engine/Exceptions/RubyException.h>
#include <Borek/Include/Engine/ZIndexAssigner.h>
#include <Borek/Include/Scripting/Ruby/Misc.h>
#include <Borek/Include/Scripting/Ruby/Modules/RBChangesExporter.h>
#include <Borek/Include/Base/Query.h>
#include <Borek/Include/Engine/EntityInitializer.h>
#include <Borek/Include/Engine/FZX/Body.h>
#include <Borek/Include/Components/IDComponent.h>
#include "Borek/Include/Components/FZXComponents.h"
#include "Borek/Include/Components/PrefabComponent.h"
#include "Borek/Include/Components/ZIndexComponent.h"
#include "Borek/Include/Debug/Log.h"
#include "Borek/Include/Events/MouseEvents.h"
#include "Borek/Include/Events/WindowEvents.h"

#include "Layers/EditorLayer.h"
#include "Layers/ProjectLayer.h"
#include "EditorSettings.h"
#include "Events/ProjectEvents.h"
#include "Layers/EditorEventHandlerLayer.h"
#include "Layers/EditorInputLayer.h"
#include "Layers/ResourceLayer.h"
#include "Misc/FontAwesome.h"
#include "Misc/Notifications/Notifications.h"
#include "Misc/SceneTabBar.h"
#include "EditorState.h"
#include "Panels/PanelEvents.h"

namespace fs = std::filesystem;

namespace Borek {

static void
generate_project(const std::filesystem::path& path)
{
        YAML::Emitter out;

        out << YAML::BeginMap;
        out << YAML::Key << "last_scene_opened_path" << YAML::Value << "";
        out << YAML::Key << "start_scene_path" << YAML::Value << "";
        out << YAML::EndMap;

        std::ofstream fout(path);
        fout << out.c_str();
}

static void
generate_imgui_ini(const std::filesystem::path& path)
{
        std::ofstream fout(path / "imgui.ini");

        fout << "[Window][Dockspace]\n";
        fout << "Pos=0,0\n";
        fout << "Size=1920,1080\n";
        fout << "Collapsed=0\n";
        fout << "\n";
        fout << "[Window][Debug##Default]\n";
        fout << "Pos=60,60\n";
        fout << "Size=400,400\n";
        fout << "Collapsed=0\n";
        fout << "\n";
        fout << "[Window][##Toolbar]\n";
        fout << "Pos=396,197\n";
        fout << "Size=221,53\n";
        fout << "Collapsed=0\n";
        fout << "\n";
        fout << "[Window][Viewport]\n";
        fout << "Pos=372,32\n";
        fout << "Size=1040,610\n";
        fout << "Collapsed=0\n";
        fout << "DockId=0x00000002,0\n";
        fout << "\n";
        fout << "[Window][Scene]\n";
        fout << "Pos=0,32\n";
        fout << "Size=370,1048\n";
        fout << "Collapsed=0\n";
        fout << "DockId=0x00000001,0\n";
        fout << "\n";
        fout << "[Window][Properties]\n";
        fout << "Pos=1414,32\n";
        fout << "Size=506,1048\n";
        fout << "Collapsed=0\n";
        fout << "DockId=0x00000006,0\n";
        fout << "\n";
        fout << "[Window][Assets]\n";
        fout << "Pos=372,644\n";
        fout << "Size=1040,436\n";
        fout << "Collapsed=0\n";
        fout << "DockId=0x00000003,0\n";
        fout << "\n";
        fout << "[Window][Repl##Repl]\n";
        fout << "Pos=355,468\n";
        fout << "Size=553,252\n";
        fout << "Collapsed=0\n";
        fout << "DockId=0x00000003,1\n";
        fout << "\n";
        fout << "[Window][Test]\n";
        fout << "Pos=55,477\n";
        fout << "Size=229,238\n";
        fout << "Collapsed=0\n";
        fout << "\n";
        fout << "[Window][Add Node]\n";
        fout << "Pos=197,99\n";
        fout << "Size=899,471\n";
        fout << "Collapsed=0\n";
        fout << "\n";
        fout << "[Window][Change Node Type]\n";
        fout << "Pos=307,165\n";
        fout << "Size=931,407\n";
        fout << "Collapsed=0\n";
        fout << "\n";
        fout << "[Window][Console]\n";
        fout << "Pos=372,644\n";
        fout << "Size=1040,436\n";
        fout << "Collapsed=0\n";
        fout << "DockId=0x00000003,1\n";
        fout << "\n";
        fout << "[Window][Dear ImGui Demo]\n";
        fout << "Pos=688,59\n";
        fout << "Size=550,680\n";
        fout << "Collapsed=1\n";
        fout << "\n";
        fout << "[Window][Editor Settings]\n";
        fout << "Pos=90,31\n";
        fout << "Size=655,335\n";
        fout << "Collapsed=0\n";
        fout << "\n";
        fout << "[Window][Import]\n";
        fout << "Pos=1414,32\n";
        fout << "Size=506,1048\n";
        fout << "Collapsed=0\n";
        fout << "DockId=0x00000006,1\n";
        fout << "\n";
        fout << "[Window][Tileset]\n";
        fout << "Pos=1414,32\n";
        fout << "Size=506,1048\n";
        fout << "Collapsed=0\n";
        fout << "DockId=0x00000006,2\n";
        fout << "\n";
        fout << "[Window][ImSearch Demo]\n";
        fout << "Pos=37,84\n";
        fout << "Size=1115,773\n";
        fout << "Collapsed=1\n";
        fout << "\n";
        fout << "[Window][SpriteSheet Form]\n";
        fout << "Pos=433,222\n";
        fout << "Size=816,536\n";
        fout << "Collapsed=0\n";
        fout << "\n";
        fout << "[Window][Tilemap Form]\n";
        fout << "Pos=984,343\n";
        fout << "Size=885,610\n";
        fout << "Collapsed=0\n";
        fout << "\n";
        fout << "[Window][Open File]\n";
        fout << "Pos=267,91\n";
        fout << "Size=1239,738\n";
        fout << "Collapsed=0\n";
        fout << "\n";
        fout << "[Window][Open Spritesheet]\n";
        fout << "Pos=518,57\n";
        fout << "Size=1128,722\n";
        fout << "Collapsed=0\n";
        fout << "\n";
        fout << "[Window][FileExplorer]\n";
        fout << "Pos=779,802\n";
        fout << "Size=360,150\n";
        fout << "Collapsed=0\n";
        fout << "\n";
        fout << "[Window][##FileExplorerPopup]\n";
        fout << "Pos=934,909\n";
        fout << "Size=255,133\n";
        fout << "Collapsed=0\n";
        fout << "\n";
        fout << "[Window][Open Project]\n";
        fout << "Pos=449,214\n";
        fout << "Size=1077,631\n";
        fout << "Collapsed=0\n";
        fout << "\n";
        fout << "[Window][Project]\n";
        fout << "Size=1920,1080\n";
        fout << "Collapsed=0\n";
        fout << "\n";
        fout << "[Window][Create Project]\n";
        fout << "Pos=379,147\n";
        fout << "Size=1306,807\n";
        fout << "Collapsed=0\n";
        fout << "\n";
        fout << "[Window][Animations]\n";
        fout << "Pos=446,644\n";
        fout << "Size=1102,436\n";
        fout << "Collapsed=0\n";
        fout << "DockId=0x00000003,2\n";
        fout << "\n";
        fout << "[Window][Animation]\n";
        fout << "Pos=372,644\n";
        fout << "Size=1040,436\n";
        fout << "Collapsed=0\n";
        fout << "DockId=0x00000003,2\n";
        fout << "\n";
        fout << "[Window][Open Animation]\n";
        fout << "Pos=371,216\n";
        fout << "Size=1148,642\n";
        fout << "Collapsed=0\n";
        fout << "\n";
        fout << "[Window][New Animation]\n";
        fout << "Pos=60,60\n";
        fout << "Size=1371,784\n";
        fout << "Collapsed=0\n";
        fout << "\n";
        fout << "[Window][Message Connection]\n";
        fout << "Pos=1576,421\n";
        fout << "Size=1045,610\n";
        fout << "Collapsed=0\n";
        fout << "\n";
        fout << "[Window][##NOTIF4]\n";
        fout << "Pos=1791,635\n";
        fout << "Size=119,79\n";
        fout << "Collapsed=0\n";
        fout << "\n";
        fout << "[Window][##NOTIF3]\n";
        fout << "Pos=1552,724\n";
        fout << "Size=358,79\n";
        fout << "Collapsed=0\n";
        fout << "\n";
        fout << "[Window][##NOTIF2]\n";
        fout << "Pos=1552,813\n";
        fout << "Size=358,79\n";
        fout << "Collapsed=0\n";
        fout << "\n";
        fout << "[Window][##NOTIF1]\n";
        fout << "Pos=1672,1380\n";
        fout << "Size=838,111\n";
        fout << "Collapsed=0\n";
        fout << "\n";
        fout << "[Window][##NOTIF0]\n";
        fout << "Pos=1672,1469\n";
        fout << "Size=838,111\n";
        fout << "Collapsed=0\n";
        fout << "\n";
        fout << "[Window][##NOTIF16]\n";
        fout << "Pos=1805,-433\n";
        fout << "Size=105,79\n";
        fout << "Collapsed=0\n";
        fout << "\n";
        fout << "[Window][##NOTIF15]\n";
        fout << "Pos=1805,-344\n";
        fout << "Size=105,79\n";
        fout << "Collapsed=0\n";
        fout << "\n";
        fout << "[Window][##NOTIF14]\n";
        fout << "Pos=1805,-255\n";
        fout << "Size=105,79\n";
        fout << "Collapsed=0\n";
        fout << "\n";
        fout << "[Window][##NOTIF13]\n";
        fout << "Pos=1805,-166\n";
        fout << "Size=105,79\n";
        fout << "Collapsed=0\n";
        fout << "\n";
        fout << "[Window][##NOTIF12]\n";
        fout << "Pos=1791,-77\n";
        fout << "Size=119,79\n";
        fout << "Collapsed=0\n";
        fout << "\n";
        fout << "[Window][##NOTIF11]\n";
        fout << "Pos=1791,12\n";
        fout << "Size=119,79\n";
        fout << "Collapsed=0\n";
        fout << "\n";
        fout << "[Window][##NOTIF10]\n";
        fout << "Pos=1305,-251\n";
        fout << "Size=605,111\n";
        fout << "Collapsed=0\n";
        fout << "\n";
        fout << "[Window][##NOTIF9]\n";
        fout << "Pos=1791,190\n";
        fout << "Size=119,79\n";
        fout << "Collapsed=0\n";
        fout << "\n";
        fout << "[Window][##NOTIF8]\n";
        fout << "Pos=1791,279\n";
        fout << "Size=119,79\n";
        fout << "Collapsed=0\n";
        fout << "\n";
        fout << "[Window][##NOTIF7]\n";
        fout << "Pos=1791,368\n";
        fout << "Size=119,79\n";
        fout << "Collapsed=0\n";
        fout << "\n";
        fout << "[Window][##NOTIF6]\n";
        fout << "Pos=1791,457\n";
        fout << "Size=119,79\n";
        fout << "Collapsed=0\n";
        fout << "\n";
        fout << "[Window][##NOTIF5]\n";
        fout << "Pos=1791,546\n";
        fout << "Size=119,79\n";
        fout << "Collapsed=0\n";
        fout << "\n";
        fout << "[Window][Select Start Scene]\n";
        fout << "Pos=60,60\n";
        fout << "Size=1306,792\n";
        fout << "Collapsed=0\n";
        fout << "\n";
        fout << "[Window][##NOTIF17]\n";
        fout << "Pos=1791,-522\n";
        fout << "Size=119,79\n";
        fout << "Collapsed=0\n";
        fout << "\n";
        fout << "[Window][Save Scene]\n";
        fout << "Pos=60,60\n";
        fout << "Size=1231,738\n";
        fout << "Collapsed=0\n";
        fout << "\n";
        fout << "[Window][Save As Scene]\n";
        fout << "Pos=60,60\n";
        fout << "Size=1306,927\n";
        fout << "Collapsed=0\n";
        fout << "\n";
        fout << "[Window][##NOTIF18]\n";
        fout << "Pos=1791,-611\n";
        fout << "Size=119,79\n";
        fout << "Collapsed=0\n";
        fout << "\n";
        fout << "[Window][Animation/##ssa_B8B9A5A5]\n";
        fout << "IsChild=1\n";
        fout << "Size=491,294\n";
        fout << "\n";
        fout << "[Table][0x252FE7CC,2]\n";
        fout << "Column 0  Weight=0.6315\n";
        fout << "Column 1  Weight=1.3685\n";
        fout << "\n";
        fout << "[Table][0x96FB997E,2]\n";
        fout << "Column 0  Weight=0.6621\n";
        fout << "Column 1  Weight=1.3379\n";
        fout << "\n";
        fout << "[Table][0xAF2B725C,2]\n";
        fout << "Column 0  Weight=0.6023\n";
        fout << "Column 1  Weight=1.3977\n";
        fout << "\n";
        fout << "[Table][0xA1340C53,2]\n";
        fout << "Column 0  Weight=1.0000\n";
        fout << "Column 1  Weight=1.0000\n";
        fout << "\n";
        fout << "[Table][0x6F53675F,2]\n";
        fout << "Column 0  Weight=1.0000\n";
        fout << "Column 1  Weight=1.0000\n";
        fout << "\n";
        fout << "[Table][0x9B620F93,2]\n";
        fout << "Column 0  Weight=1.0000\n";
        fout << "Column 1  Weight=1.0000\n";
        fout << "\n";
        fout << "[Table][0xD9101FA3,2]\n";
        fout << "Column 0  Weight=1.0000\n";
        fout << "Column 1  Weight=1.0000\n";
        fout << "\n";
        fout << "[Table][0xA75A0487,2]\n";
        fout << "Column 0  Weight=1.0000\n";
        fout << "Column 1  Weight=1.0000\n";
        fout << "\n";
        fout << "[Table][0x2C153EC6,2]\n";
        fout << "Column 0  Weight=1.0000\n";
        fout << "Column 1  Weight=1.0000\n";
        fout << "\n";
        fout << "[Table][0xAC4775A9,2]\n";
        fout << "Column 0  Weight=1.0000\n";
        fout << "Column 1  Weight=1.0000\n";
        fout << "\n";
        fout << "[Table][0xBA494C88,2]\n";
        fout << "Column 0  Weight=1.0000\n";
        fout << "Column 1  Weight=1.0000\n";
        fout << "\n";
        fout << "[Docking][Data]\n";
        fout << "DockSpace       ID=0x382E4429 Window=0x260A4489 Pos=0,32 Size=1920,1048 Split=X Selected=0xE47D7937\n";
        fout << "  DockNode      ID=0x00000005 Parent=0x382E4429 SizeRef=2012,690 Split=X\n";
        fout << "    DockNode    ID=0x00000001 Parent=0x00000005 SizeRef=370,690 Selected=0xE601B12F\n";
        fout << "    DockNode    ID=0x00000004 Parent=0x00000005 SizeRef=1640,690 Split=Y\n";
        fout << "      DockNode  ID=0x00000002 Parent=0x00000004 SizeRef=1288,610 CentralNode=1 Selected=0xC450F867\n";
        fout << "      DockNode  ID=0x00000003 Parent=0x00000004 SizeRef=1288,436 Selected=0x5921A509\n";
        fout << "  DockNode      ID=0x00000006 Parent=0x382E4429 SizeRef=506,690 Selected=0x7E5E694D\n";
}

ImFont* AddFont(std::string_view path, float size)
{
        ImGuiIO& io = ImGui::GetIO();

        static const ImWchar icons_ranges[] = {ICON_MIN_FA, ICON_MAX_FA, 0};
        ImFontConfig icons_config;
        icons_config.MergeMode = true;
        icons_config.PixelSnapH = true;

        ImFont* res = io.Fonts->AddFontFromFileTTF(path.data(), size);
        io.Fonts->AddFontFromFileTTF(ASSET_PATH("assets/Fonts/Font Awesome1.otf"), size, &icons_config, icons_ranges);
        io.Fonts->AddFontFromFileTTF(ASSET_PATH("assets/Fonts/Font Awesome2.otf"), size, &icons_config, icons_ranges);
        io.Fonts->AddFontFromFileTTF(ASSET_PATH("assets/Fonts/Font Awesome3.otf"), size, &icons_config, icons_ranges);
        return res;
}

static Ref<Graphics::FrameBuffer> fb;

class Stavitel : public Application
{
public:
        Stavitel() : Borek::Application("Borek Stavitel!")
        {
                SwitchProjectEvent::AddListener(EVENT_FN(SwitchProject));
                MouseButtonEvent::AddListener(EVENT_FN(OnMouseButton));
                WindowResizeEvent::AddListener(EVENT_FN(OnWindowResize));
                Graphics::FrameBufferSettings fb_settings;
                fb_settings.width = 320;
                fb_settings.height = 180;
                fb = Graphics::FrameBuffer::Create(fb_settings);
                m_EditorInputLayer = new EditorInputLayer();
                PushLayer(m_EditorInputLayer);
                m_ResourceLayer = new ResourceLayer();
                PushLayer(m_ResourceLayer);
                ImGuiIO& io = ImGui::GetIO();

                AddFont(ASSET_PATH("assets/Fonts/JetBrainsMono-Bold.ttf"), 24.0f);
                io.FontDefault = AddFont(ASSET_PATH("assets/Fonts/JetBrainsMono-Medium.ttf"), 24.0f);
                AddFont(ASSET_PATH("assets/Fonts/JetBrainsMono-Medium.ttf"), 32.0f);
                AddFont(ASSET_PATH("assets/Fonts/JetBrainsMono-Bold.ttf"), 128.0f);
                
                s_LogFunc = [this](const std::string& str) {
                        std::stringstream ss;
                        ss << str << "\033[39m\033[49m";
                        std::string res = ss.str();
                        glm::vec4 color;
                        switch (str[3]) {
                        case '1':
                                color = Colors::RED;
                                break;
                        case '2':
                                color = Colors::GREEN;
                                break;
                        case '3':
                                color = Colors::YELLOW;
                                break;
                        case '4':
                                color = Colors::CYAN;
                                break;
                        case '6':
                                color = Colors::BLUE;
                                break;
                        }
                        m_EditorLayer->GetConsole().Add(res.substr(10, res.size() - 20), color);
                        std::cout << res << "\n";
                };

                s_LogFunc = [](const std::string& str) {
                        std::stringstream ss;
                        ss << str << "\033[39m\033[49m";
                        std::string res = ss.str();
                        std::cout << res << "\n";
                };
        }

        ~Stavitel()
        {
                SceneTabBar::Deinit();
        }

        void OnUpdate(float delta) override
        {
                if (EditorState::game_state == GameState::kRestarting) {
                        m_EditorLayer->OnGameEnded();
                        auto path = Project::Instance().last_scene_opened_path;
                        SceneTabBar::ChangeScene(path, true);
                        m_EditorLayer->OnGameStarted();
                        m_Started = true;
                        EditorState::game_state = GameState::kPlaying;
                        m_CurrentScene->Initialize();

                } else if (IsPlaying() && !m_Started) {
                        m_EditorLayer->OnGameStarted();
                        SceneTabBar::SaveScene();
                        m_Started = true;
                        m_CurrentScene->Initialize();
                } else if (!IsPlaying() && m_Started) {
                        m_EditorLayer->OnGameEnded();
                        auto path = Project::Instance().last_scene_opened_path;
                        BOREK_ENGINE_INFO("Changing to scene: {}", path.string());
                        SceneTabBar::ChangeScene(path, true);
                        m_Started = false;
                        m_SpriteGrid.Clear();
                }

                Application::OnUpdate(delta);
        }

        virtual void OnImGuiRenderBegin() override
        {
                Application::OnImGuiRenderBegin();

                if (m_EditorLayer)
                        m_EditorLayer->BeginDockspace();
        }

        virtual void OnImguiRenderEnd() override
        {
                Application::OnImguiRenderEnd();

                if (m_EditorLayer)
                        m_EditorLayer->EndDockspace();
        }

        virtual std::pair<glm::vec2, glm::vec2>
        GetMouseOffsetInternal() override
        {
                if (!m_EditorLayer)
                        return Application::GetMouseOffsetInternal();

                return { m_EditorLayer->GetViewportPosition(),
                         m_EditorLayer->GetViewportSize() };
        }

        virtual void
        BeforeRenderEnd() override
        {
                if (IsPlaying() && !EditorSettings::show_collision_shapes) {
                        return;
                }

                for (auto& [id, area] : Query<IDComponent, AreaComponent>()) {
                        auto tran = Entity(id->ecs_id).GlobalTransform();
                        Drawing::Quad::Draw(tran.position, scale_tof(tran.scale) * glm::vec2(area->rc.size_x, area->rc.size_y),
                                            Color(0, 255, 212, 80),
                                            ZIndexAssigner::GetTop());
                }

                for (auto& [id, body] : Query<IDComponent, BodyComponent>()) {
                        auto tran = Entity(id->ecs_id).GlobalTransform();
                        Drawing::Quad::Draw(tran.position, scale_tof(tran.scale) * glm::vec2(body->rc.size_x, body->rc.size_y),
                                            Color(0, 255, 255, 80),
                                            ZIndexAssigner::GetTop());
                }

                for (auto& [id, tmap] : Query<IDComponent, TilemapComponent>()) {
                        if (!tmap->tilemap.IsValid())
                                continue;

                        auto tran = Entity(id->ecs_id).GlobalTransform();
                        const glm::vec2& scale = scale_tof(tran.scale);

                        for (auto it : tmap->items) {
                                const auto& pos = it.first;
                                if (!tmap->tilemap->tile_coliders[it.second].Exists())
                                        continue;

                                Drawing::Quad::Draw({ pos.second * tmap->step_x * scale.x,
                                                      pos.first * tmap->step_y * scale.y },
                                                    { tmap->tilemap->sprite_sheet->step_x * scale.x * 1.001f,
                                                      tmap->tilemap->sprite_sheet->step_y * scale.y * 1.001f },
                                                    Color(0, 255, 255, 80), ZIndexAssigner::GetTop());
                        }
                }
        }



        void SetCamera() override
        {
                if (IsPlaying()) {
                        Application::SetCamera();
                } else {
                        m_Camera = &m_EditorInputLayer->GetCamera();
                        m_CameraTransform = m_EditorInputLayer->GetCameraTransform();
                }
        }

        virtual bool Playing() override
        {
                return EditorState::game_state == GameState::kPlaying;
        }

        bool
        SwitchProject(SwitchProjectEvent& e)
        {
                m_ProjectPath = e.GetPath();
                WITH_CHANGE(Utils::Settings::InstanceM(), {
                        Utils::Settings::InstanceM().current_project_path = m_ProjectPath;
                });
                if (m_ProjectPath == "") {
                        if (!m_ProjectLayer) {
                                m_ProjectLayer = new ProjectLayer();
                        }
                        PushLayer(m_ProjectLayer);

                        if (m_EditorLayer) {
                                m_Layers.Pop(m_EditorLayer);
                                m_Layers.Pop(m_EditorEventHandlerLayer);
                                delete m_EditorLayer;
                        }
                } else {
                        if (m_EditorLayer) {
                                m_Layers.Pop(m_EditorLayer);
                                delete m_EditorLayer;
                        }

                        m_EditorLayer = new EditorLayer();
                        if (!m_EditorEventHandlerLayer)
                                m_EditorEventHandlerLayer = new EditorEventHandlerLayer();

                        PushLayer(m_EditorLayer);
                        PushLayer(m_EditorEventHandlerLayer);
                        if (m_ProjectLayer) {
                                m_Layers.Pop(m_ProjectLayer);
                        }
                }

                return true;
        }

        void LoadProject() override
        {
                m_ProjectPath = fs::current_path();
                fs::path pth = fs::path(m_ProjectPath) / "project.bproj";
                if (!fs::exists(pth)) {
                        std::cout << "Could not find project file. Do you want to create a new project? ([y] or n): ";
                        std::string input;
                        std::getline(std::cin, input);
                        if (input.empty() || input == "y") {
                                std::cout << "Generating project\n";
                                generate_project(pth);
                                generate_imgui_ini(m_ProjectPath);
                        } else {
                                std::cout << "Exitting\n";
                                exit(1);
                        }
                }
                Project::Deserialize(std::filesystem::path(m_ProjectPath) / "project.bproj");
                SendEvent<SwitchProjectEvent>(m_ProjectPath);
        }

        void ChangeSceneFunc(const std::string& path) override
        {
                if (!IsPlaying()) {
                        SceneTabBar::ChangeScene(path);
                        Project::InstanceM()->last_scene_opened_path = path;
                } else {
                        Application::ChangeSceneFunc(path);
                        SceneTabBar::RuntimeChangeScene(m_CurrentScene);
                }
        }

        void
        OnRenderEnd() override
        {
                m_FrameBuffer->Unbind();
        }

        bool
        HandleRubyException(const RubyException&) override
        {
                mrb_state* mrb = m_RubyEngine->GetRubyVM();
                RBModules::ChangesExporter::Reset(mrb);
                m_EditorLayer->OnGameEnded();
                auto path = Project::Instance().last_scene_opened_path;
                SceneTabBar::ChangeScene(path, true);
                m_Started = false;
                EditorState::game_state = GameState::kStopped;
                BOREK_ENGINE_ERROR("{}", get_mrb_exc_text());
                Notifications::Add(Notification()
                        .Type(NotificationType_Error)
                        .Text("Error happened during execution, stopping game. "
                              "Check log for trace.")
                        .Title("Ruby Error"));
                return true;
        }

        bool
        OnMouseButton(MouseButtonEvent& e)
        {
                glm::vec2 mpos_rel = glm::abs(Input::GetMousePosRelative());
                if (mpos_rel.x > 1.0f || mpos_rel.y >= 1.0f)
                        return false;

                if (!IsPlaying() && e.IsPressed() && e.GetButton() == MouseButton::BUTTON_LEFT) {
                        FZX::SmallList<uint32_t> res;
                        glm::vec2 pos = Input::GetMouseWorldPos() - m_CameraTransform.position;
                        m_SpriteGrid.GetCollisions(pos, UINT32_MAX, &res);

                        if (res.size() == 0)
                                return false;

                        float zindex = std::numeric_limits<float>::min();
                        Entity top;
                        for (Entity e : res) {
                                float ezindex = e.GetComponent<ZIndexComponent>().value;
                                if (ezindex > zindex) {
                                        zindex = ezindex;
                                        top = e;
                                }
                        }

                        Entity parent = top.GetParent();
                        while (!parent.IsNil()) {
                                if (parent.HasComponent<PrefabComponent>())
                                        top = parent;

                                parent = parent.GetParent();
                        }

                        Application::SendEvent<ChangeEntityEvent>(top);
                }

                return true;
        }

        bool OnWindowResize(WindowResizeEvent& e)
        {
                m_EditorInputLayer->GetCamera().OnWindowResized(e);
                return true;
        }

        EditorLayer* m_EditorLayer = nullptr;
        ResourceLayer* m_ResourceLayer = nullptr;
        EditorInputLayer* m_EditorInputLayer = nullptr;
        EditorEventHandlerLayer* m_EditorEventHandlerLayer = nullptr;
        ProjectLayer* m_ProjectLayer = nullptr;

        bool m_Started = false;

        bool m_IsDragging = false;
};

}  // namespace Borek

BOREK_APPLICATION(Borek::Stavitel)
