// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include <algorithm>

#include "Include/Base/LayerStack.h"

namespace Borek {

LayerStack::LayerStack()
{
}

LayerStack::~LayerStack()
{
        for (auto layer : m_Layers) {
                layer->OnDetach();
                delete layer;
        }
}

bool LayerStack::PopOverlay(Layer* layer)
{
        auto l = std::find(m_Layers.begin(), m_Layers.end(), layer);
        if (l != m_Layers.end()) {
                m_Layers.erase(l);
                return true;
        }

        return false;
}

}  // namespace Borek
