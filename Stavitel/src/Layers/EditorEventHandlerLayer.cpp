// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Layers/EditorEventHandlerLayer.h"
#include "Events/Events.h"
#include "Include/Base/Application.h"
#include "Include/Base/Entity.h"
#include "Misc/SceneTabBar.h"
#include "Panels/PanelEvents.h"
#include <Borek/Include/Events/SceneEvents.h>
#include <Borek/Include/Events/ComponentEvents.h>
#include <Borek/Include/Events/EntityEvents.h>

namespace Borek {

static bool
on_scene_panel_selected(ScenePanelSelectedEvent& e)
{
        Application::SendEventImmediate<ChangeEntityEvent>(e.GetEntity());
        return true;
}

static bool
on_destroy_entity(DestroyEntityEvent& e)
{
        Application::SendEventImmediate<ChangeEntityEvent>(Entity());
        return true;
}

static bool
on_scene_changed(SceneChangedEvent& e)
{
        Application::SendEventImmediate<ChangeEntityEvent>(Entity());
        return true;
}

static bool
on_add_component(AddComponentEvent& e)
{
        e.GetEntity().AddComponent(e.GetId());
        return true;
}

static bool
on_remove_component(RemoveComponentEvent& e)
{
        e.GetEntity().RemoveComponent(e.GetId());
        return true;
}

static bool
on_editor_command(EditorCommandEvent& e)
{
        SceneTabBar::GetCommandHandler().AddCommand(std::move(e.GetCommand()));
        return true;
}

void
EditorEventHandlerLayer::OnAttach()
{
        m_EventHandles[0] = ScenePanelSelectedEvent::AddListener(on_scene_panel_selected);
        m_EventHandles[1] = DestroyEntityEvent::AddListener(on_destroy_entity);
        m_EventHandles[2] = SceneChangedEvent::AddListener(on_scene_changed);
        m_EventHandles[3] = AddComponentEvent::AddListener(on_add_component);
        m_EventHandles[4] = RemoveComponentEvent::AddListener(on_remove_component);
        m_EventHandles[5] = EditorCommandEvent::AddListener(on_editor_command);
        EditorCommandEvent::Grab(m_EventHandles[5]);
}

void
EditorEventHandlerLayer::OnDetach()
{
        ScenePanelSelectedEvent::RemoveListener(m_EventHandles[0]);
        DestroyEntityEvent::RemoveListener(m_EventHandles[1]);
        SceneChangedEvent::RemoveListener(m_EventHandles[2]);
        AddComponentEvent::RemoveListener(m_EventHandles[3]);
        RemoveComponentEvent::RemoveListener(m_EventHandles[4]);
        EditorCommandEvent::Release(m_EventHandles[5]);
        EditorCommandEvent::RemoveListener(m_EventHandles[5]);

}

}  // namespace Borek
