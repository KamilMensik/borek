// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <string>

#include "Include/Events/Event.h"

namespace Borek {

class Layer {
public:
        Layer(const std::string& name = "Layer");
        virtual ~Layer();

        virtual void OnAttach() {}
        virtual void OnDetach() {}
        virtual void OnUpdate(float delta) {}
        virtual void OnImGuiRender() {}

        inline const std::string& GetName()
        {
                return m_Name;
        }

protected:
        std::string m_Name;
};

}  // namespace Borek
