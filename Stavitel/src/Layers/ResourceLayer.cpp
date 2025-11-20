// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Layers/ResourceLayer.h"
#include "Events/ResourceEvents.h"
#include "Include/Base/Application.h"
#include "Include/Base/Query.h"
#include "Include/Components/IDComponent.h"
#include "Include/Components/PrefabComponent.h"
#include "Include/Engine/Assets/Asset.h"
#include "Include/Engine/Utils/PathHelpers.h"
#include "Include/Engine/Utils/Settings.h"
#include "Include/Engine/Utils/StringHelpers.h"
#include "Misc/EditorTextures.h"

namespace Borek {

namespace fs = std::filesystem;

static bool
on_resource_renamed(ResourceRenamedEvent& ev)
{
        fs::path og_asset = ev.GetFrom();

        switch (Hash(ev.GetFrom().extension())) {
        case Hash(".tex"):
                og_asset.replace_extension("png");
                if (fs::exists(og_asset))
                        break;

                og_asset.replace_extension("jpg");
                if (fs::exists(og_asset))
                        break;

                og_asset.replace_extension("jpeg");
                if (fs::exists(og_asset))
                        break;

                return true;
        case Hash(".scr"):
                og_asset.replace_extension("png");
                if (fs::exists(og_asset))
                        break;

                return true;
        case Hash(".snd"):
                og_asset.replace_extension("mp3");
                if (fs::exists(og_asset))
                        break;

                og_asset.replace_extension("waw");
                if (fs::exists(og_asset))
                        break;

                og_asset.replace_extension("flac");
                if (fs::exists(og_asset))
                        break;

                return true;
        case Hash(".fnt"):
                og_asset.replace_extension("ttf");
                if (fs::exists(og_asset))
                        break;

                return true;
        default:
                return true;
        }

        fs::rename(og_asset, ev.GetTo().parent_path() / og_asset.filename());
        AssetManager::RefreshPath(Utils::Path::ToRelative(ev.GetFrom()),
                                  Utils::Path::ToRelative(ev.GetTo()));

        return true;
}

void
ResourceLayer::OnUpdate(float delta)
{
        if (--m_TickCounter <= 0) {
                ResourceAssetifier::AssetifyFolder(Utils::Settings::ProjectPath());
                for (auto& [id, prefab] : Query<IDComponent, PrefabComponent>()) {
                        prefab->Update(id->ecs_id);
                }
                m_TickCounter = 30;
        }
}

void
ResourceLayer::OnAttach()
{
        ResourceAssetifier::AssetifyFolder(Utils::Settings::ProjectPath());
        EditorTextures::Init();
        m_EventHandles[0] = ResourceRenamedEvent::AddListener(on_resource_renamed);
}

void
ResourceLayer::OnDetach()
{
        EditorTextures::Deinitialize();
        ResourceRenamedEvent::RemoveListener(m_EventHandles[0]);
}

}  // namespace Borek
