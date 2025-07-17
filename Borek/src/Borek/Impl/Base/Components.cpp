// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Base/Components.h"
#include "Include/Base/Application.h"
#include "Include/Debug/Log.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/trigonometric.hpp"

#include <mruby.h>
#include <mruby/value.h>
#include <mrbcpp.h>

namespace Borek {

using Cam = CameraComponent;
using Trans = TransformComponent;

constexpr static glm::vec2 FromPixels(const glm::vec2& pixels) {
        return pixels / 400.0f;
}

Trans& Trans::Move(float x, float y)
{
        position.x += x;
        position.y += y;
        return *this;
}

Trans& Trans::Move(const glm::vec2& xy)
{
        position += xy;
        return *this;
}

glm::mat4 Trans::GetTransformMat(const glm::vec2& offset) const
{
        glm::mat4 transform = glm::translate(glm::mat4(1.0f),
                                             glm::vec3(position + offset, 0.0f));

        if (rotation.x != 0) {
                transform = glm::rotate(transform, glm::radians(rotation.x),
                                        glm::vec3(1.0f, 0.0f, 0.0f));
        }
        if (rotation.y != 0) {
                transform = glm::rotate(transform, glm::radians(rotation.y),
                                        glm::vec3(0.0f, 1.0f, 0.0f));
        }
        if (rotation.z != 0) {
                transform = glm::rotate(transform, glm::radians(rotation.z),
                                        glm::vec3(0.0f, 0.0f, 1.0f));
        }

        return glm::scale(transform, glm::vec3(scale, 1.0f));
}

glm::mat4 Trans::GetPixelTransformMat(const glm::vec2& offset) const
{
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(FromPixels(position + offset), 0));

        if (rotation.x != 0) {
                transform = glm::rotate(transform, glm::radians(rotation.x),
                                        glm::vec3(1.0f, 0.0f, 0.0f));
        }
        if (rotation.y != 0) {
                transform = glm::rotate(transform, glm::radians(rotation.y),
                                        glm::vec3(0.0f, 1.0f, 0.0f));
        }
        if (rotation.z != 0) {
                transform = glm::rotate(transform, glm::radians(rotation.z),
                                        glm::vec3(0.0f, 0.0f, 1.0f));
        }

        return glm::scale(transform, glm::vec3(FromPixels(scale), 0.0f));
}


Cam::CameraComponent(float aspect_ratio, float zoom)
        : aspect_ratio(aspect_ratio), zoom(zoom),
        camera(-aspect_ratio * zoom, aspect_ratio * zoom, -zoom, zoom),
        is_active(false)
{
        BOREK_LOG_INFO("Set aspect ratio to: {}", aspect_ratio);
}

const glm::mat4& Cam::GetViewProjectionMatrix(const Trans& transform)
{
        Recalculate(transform);

        return camera.GetViewProjectionMatrix();
}

bool Cam::OnWindowResized(WindowResizeEvent& ev)
{
        aspect_ratio = ((float)ev.GetWidth() / ev.GetHeight());
        camera.SetProjection(-aspect_ratio * zoom, aspect_ratio * zoom,
                               -zoom, zoom);
        return false;
}

void Cam::Recalculate(const Trans& transform)
{
        camera.SetProjection(-aspect_ratio * zoom, aspect_ratio * zoom,
                             -zoom, zoom);
        camera.Recalculate(glm::vec3(transform.position, 0.0f), transform.rotation);
}

RubyScriptComponent::RubyScriptComponent()
{
}

void RubyScriptComponent::Initialize(Entity e)
{
        if (ruby_class.empty())
                return;

        mrb_state* mrb = Application::GetRubyEngine().GetRubyVM();
        if (!s_RubyEntityClass)
                s_RubyEntityClass = mrb_class_get_under(mrb, Application::GetRubyEngine().GetBorekModule(), "Entity");

        mrb_value entity_id = mrb_fixnum_value(e.GetId());
        mrb_value ruby_entity = mrb_class_new_instance(mrb, 1, &entity_id, s_RubyEntityClass);
        mrb_value instance = mrb_class_new_instance(mrb, 1, &ruby_entity, mrb_class_get(mrb, ruby_class.c_str()));
        ruby_instance = instance.w;
}

void RubyScriptComponent::OnUpdate(float delta)
{
        if (ruby_class.empty())
                return;

        mrb_value instance = { ruby_instance };
        mrb_state* mrb = Application::GetRubyEngine().GetRubyVM();

        MRB_FUNCALL(instance, "on_update", MRB_FLOAT(delta));
}

void RubyScriptComponent::OnDestroy()
{
        if (ruby_class.empty())
                return;

        mrb_value instance = { ruby_instance };
        mrb_state* mrb = Application::GetRubyEngine().GetRubyVM();

        MRB_FUNCALL(instance, "on_destroy");
}

RClass* RubyScriptComponent::s_RubyEntityClass = nullptr;

}  // namespace Borek
