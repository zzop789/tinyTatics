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
        m_Shader = std::make_unique<TinyTactics::Shader>("Assets/Shaders/FlatColor.vert", "Assets/Shaders/FlatColor.frag");
        m_Shader->Bind();
        TinyTactics::Shader::Unbind();
    }

    void BaseLayer::OnDetach()
    {
        // Flow: Application shutdown -> LayerStack::OnDetach -> release OpenGL-owned Shader.
        m_Shader.reset();
    }

    void BaseLayer::OnUpdate(TinyTactics::TimeStep timestep)
    {
        // Flow: Application::Run -> BaseLayer::OnUpdate -> future Sandbox rendering/gameplay.
        (void)timestep;
    }
}
