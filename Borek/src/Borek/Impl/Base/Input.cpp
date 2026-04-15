// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include <unordered_map>
#include <cstdint>

#include "Include/Base/Application.h"
#include "Include/Events/KeyEvents.h"
#include "Include/Base/Input.h"

namespace Borek {

static std::unordered_map<KeyCode, uint64_t> input_pressed_at;

static void on_key(KeyEvent& e)
{
        if (!e.IsPressed() || e.IsRepeated())
                return;

        input_pressed_at[e.GetKeycode()] = Application::GetTick();
}

void
Input::Init()
{
        KeyEvent::AddListener(on_key);
}

glm::vec2 Input::GetAxis()
{
        using KC = KeyCode;

        return glm::vec2(
                IsKeyPressed(KC::A) * -1.0f + IsKeyPressed(KC::D) * 1.0f,
                IsKeyPressed(KC::S) * -1.0f + IsKeyPressed(KC::W) * 1.0f);
}

bool
Input::IsKeyJustPressed(KeyCode key)
{
        auto it = input_pressed_at.find(key);
        if (it == input_pressed_at.end() || it->second != Application::GetTick())
                return false;

        return true;
}

}
