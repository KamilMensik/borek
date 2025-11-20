// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Layers/EditorEventHandlerLayer.h"
#include "Events/Events.h"
#include "Include/Base/Application.h"
#include "Include/Base/Entity.h"
#include "Panels/PanelEvents.h"
#include <Borek/Include/Events/SceneEvents.h>
#include <Borek/Include/Events/ComponentEvents.h>

namespace Borek {

static void
on_scene_panel_selected(ScenePanelSelectedEvent& e)
{
        Application::SendEvent<ChangeEntityEvent>(e.GetEntity());
}

static void
on_remove_entity(RemoveEntityEvent& e)
{
        Application::SendEvent<ChangeEntityEvent>(Entity());
        e.GetEntity().Delete();
}

static void
on_scene_changed(SceneChangedEvent& e)
{
        Application::SendEvent<ChangeEntityEvent>(Entity());
}

static void
on_add_component(AddComponentEvent& e)
{
        e.GetEntity().AddComponent(e.GetId());
}

static void
on_remove_component(RemoveComponentEvent& e)
{
        e.GetEntity().RemoveComponent(e.GetId());
}

void
EditorEventHandlerLayer::OnAttach()
{
        m_EventHandles[0] = ScenePanelSelectedEvent::AddListener(on_scene_panel_selected);
        m_EventHandles[1] = RemoveEntityEvent::AddListener(on_remove_entity);
        m_EventHandles[2] = SceneChangedEvent::AddListener(on_scene_changed);
        m_EventHandles[3] = AddComponentEvent::AddListener(on_add_component);
        m_EventHandles[4] = RemoveComponentEvent::AddListener(on_remove_component);
}

void
EditorEventHandlerLayer::OnDetach()
{
        ScenePanelSelectedEvent::RemoveListener(m_EventHandles[0]);
        RemoveEntityEvent::RemoveListener(m_EventHandles[1]);
        SceneChangedEvent::RemoveListener(m_EventHandles[2]);
        AddComponentEvent::RemoveListener(m_EventHandles[3]);
        RemoveComponentEvent::RemoveListener(m_EventHandles[4]);
}

}  // namespace Borek
