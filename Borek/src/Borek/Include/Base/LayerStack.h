// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <vector>

#include "Include/Core.h"
#include "Include/Base/Layer.h"

namespace Borek {

class LayerStack {
public:
        LayerStack();
        ~LayerStack();
        inline void Push(Layer* layer)
        {
                m_Layers.insert(m_Layers.begin() + m_LayersCount, layer);
                m_LayersCount++;
        }
        inline void Pop(Layer* layer)
        {
                if (PopOverlay(layer)) m_LayersCount--;
        }
        inline void PushOverlay(Layer* layer)
        {
                m_Layers.emplace_back(layer);
        }
        bool PopOverlay(Layer*);

        inline auto begin() { return m_Layers.begin(); }
        inline auto end() { return m_Layers.end(); }

private:
        unsigned m_LayersCount = 0;
        std::vector<Layer*> m_Layers;
};

}  // namespace Borek
