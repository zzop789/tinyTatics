#pragma once

#include "Engine/Core/TimeStep.h"

#include <string>

namespace TinyTactics
{
    // Base class for app/game/editor code that runs inside Application.
    // Flow: Application::Run -> Layer::OnUpdate -> game/render logic.
    class Layer
    {
    public:
        explicit Layer(std::string debugName = "Layer");
        virtual ~Layer();

        Layer(const Layer&) = delete;
        Layer& operator=(const Layer&) = delete;

        // Calls: LayerStack invokes this after taking ownership.
        virtual void OnAttach() {}

        // Calls: LayerStack invokes this before removing or destroying the layer.
        virtual void OnDetach() {}

        // Calls: Application will call this once per frame.
        virtual void OnUpdate(TimeStep timestep) {}

        const std::string& GetDebugName() const;

    private:
        std::string m_DebugName;
    };
}
