#include "Engine/Core/LayerStack.h"

#include <algorithm>
#include <stdexcept>
#include <utility>

namespace TinyTactics
{
    LayerStack::~LayerStack()
    {
        // Flow: Application shutdown -> LayerStack destruction -> Layer::OnDetach.
        for (auto& layer : m_Layers)
        {
            layer->OnDetach();
        }
    }

    Layer& LayerStack::PushLayer(std::unique_ptr<Layer> layer)
    {
        if (!layer)
        {
            throw std::invalid_argument("LayerStack::PushLayer received null layer.");
        }

        Layer* rawLayer = layer.get();
        m_Layers.emplace_back(std::move(layer));

        // Calls: layer gets initialization hook after ownership is stored.
        rawLayer->OnAttach();
        return *rawLayer;
    }

    void LayerStack::PopLayer(const Layer* layer)
    {
        auto iterator = std::find_if(m_Layers.begin(), m_Layers.end(),
            [layer](const std::unique_ptr<Layer>& storedLayer)
            {
                return storedLayer.get() == layer;
            });

        if (iterator == m_Layers.end())
        {
            return;
        }

        // Calls: detach before unique_ptr erases and destroys the layer.
        (*iterator)->OnDetach();
        m_Layers.erase(iterator);
    }
}
