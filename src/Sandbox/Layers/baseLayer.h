#pragma once

#include "Engine/Core/core.h"

namespace TinyTacticsSandbox
{
    // Sandbox-side layer used to test the engine Application -> LayerStack path.
    // Flow: main -> Application::PushLayer -> BaseLayer::OnUpdate.
    // Jump: put early gameplay/rendering experiments here before moving systems into Engine.
    class BaseLayer final : public TinyTactics::Layer
    {
    public:
        BaseLayer();
        ~BaseLayer() override;

        // Calls: LayerStack after the layer is owned by Application.
        void OnAttach() override;

        // Calls: Application::Run once per frame with real frame timing.
        void OnUpdate(TinyTactics::TimeStep timestep) override;
    };
}
