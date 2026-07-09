#pragma once

#include "Engine/Core/Layer.h"

#include <memory>
#include <vector>

namespace TinyTactics
{
    // Owns app/editor/game layers in their update order.
    // Flow: Application::PushLayer -> LayerStack::PushLayer -> Layer::OnAttach.
    // Jump: Application::Run iterates this stack once per frame.
    class LayerStack
    {
    public:
        LayerStack() = default;
        ~LayerStack();

        LayerStack(const LayerStack&) = delete;
        LayerStack& operator=(const LayerStack&) = delete;

        // Takes ownership of a layer created by the application side.
        Layer& PushLayer(std::unique_ptr<Layer> layer);

        // Removes a layer by borrowed address; ownership remains inside LayerStack until erase.
        void PopLayer(const Layer* layer);

        // Flow: Application shutdown -> LayerStack::Clear -> Layer::OnDetach + destruction.
        void Clear();

        using LayerContainer = std::vector<std::unique_ptr<Layer>>;
        using Iterator = LayerContainer::iterator;
        using ConstIterator = LayerContainer::const_iterator;

        Iterator begin() { return m_Layers.begin(); }
        Iterator end() { return m_Layers.end(); }
        ConstIterator begin() const { return m_Layers.begin(); }
        ConstIterator end() const { return m_Layers.end(); }

    private:
        LayerContainer m_Layers;
    };
}
