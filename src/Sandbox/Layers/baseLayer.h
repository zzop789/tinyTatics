#pragma once

#include "Engine/Core/core.h"
#include <memory>
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

        // Calls: LayerStack before Application destroys the OpenGL window/context.
        void OnDetach() override;

        // Calls: Application::Run once per frame with real frame timing.
        void OnUpdate(TinyTactics::TimeStep timestep) override;
    private:
        // Sandbox-specific resources for testing gameplay/rendering.
        TinyTactics::Camera2D m_Camera;
        std::unique_ptr<TinyTactics::Texture2D> m_Texture;
    };
}
