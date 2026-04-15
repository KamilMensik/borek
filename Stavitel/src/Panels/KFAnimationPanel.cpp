// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include <imgui.h>
#include <im-neo-sequencer/imgui_neo_sequencer.h>

#include "Panels/KFAnimationPanel.h"

namespace Borek {
namespace Panels {

KFAnimationPanel::KFAnimationPanel()
{

}

KFAnimationPanel::~KFAnimationPanel()
{

}

void
KFAnimationPanel::OnImguiRender()
{
        if (!ImGui::Begin("Animations")) {
                ImGui::End();
                return;
        }

        static int32_t currentFrame = 0;
        static int32_t startFrame = -10;
        static int32_t endFrame = 64;
        static bool transformOpen = false;
        bool do_delete = false;

        if(ImGui::BeginNeoSequencer("Sequencer", &currentFrame, &startFrame, &endFrame,
                                    ImGui::GetContentRegionAvail(), ImGuiNeoSequencerFlags_Selection_EnableDragging |
                                    ImGuiNeoSequencerFlags_EnableSelection | ImGuiNeoSequencerFlags_Selection_EnableDeletion)) {
                if(ImGui::BeginNeoGroup("Transform",&transformOpen)) {
                        static std::vector<ImGui::FrameIndexType> keys = {0, 10, 24};
                        if(ImGui::BeginNeoTimeline("Position", keys )) {
                                ImGui::EndNeoTimeLine();
                        }
                        ImGui::EndNeoGroup();
                }

                ImGui::EndNeoSequencer();
        }

        ImGui::End();
}

}  // namespace Panels
}  // namespace Borek
