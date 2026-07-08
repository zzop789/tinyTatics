#include "Sandbox/Layers/baseLayer.h"

namespace TinyTacticsSandbox
{
    BaseLayer::BaseLayer()
        : TinyTactics::Layer("BaseLayer")
    {
    }

    BaseLayer::~BaseLayer() = default;

    void BaseLayer::OnAttach()
    {
        // Flow: Application startup -> LayerStack::PushLayer -> BaseLayer resource setup.
    }

    void BaseLayer::OnUpdate(TinyTactics::TimeStep timestep)
    {
        // Flow: Application::Run -> BaseLayer::OnUpdate -> future Sandbox rendering/gameplay.
        (void)timestep;
    }
}
