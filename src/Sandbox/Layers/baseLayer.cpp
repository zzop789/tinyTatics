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
        m_Camera.SetViewportSize(1280.0F, 720.0F);
        m_Camera.SetPosition({ 0.0F, 0.0F }); //max [-1,1] in both axes
        m_Camera.SetZoom(1.0F);
        m_Texture = std::make_unique<TinyTactics::Texture2D>("Assets/Textures/bronze_128.png");
    }

    void BaseLayer::OnDetach()
    {
        // Flow: Application shutdown -> LayerStack::OnDetach -> release OpenGL-owned resources.
        m_Texture.reset();
    }

    void BaseLayer::OnUpdate(TinyTactics::TimeStep timestep)
    {
        // Flow: Application::Run -> BaseLayer::OnUpdate -> Shader + VAO -> indexed draw.
        (void)timestep;
        TinyTactics::Renderer2D::BeginScene(m_Camera);
        TinyTactics::Renderer2D::DrawQuad({ 0.0F, 0.0F }, { 1.0F, 1.0F }, {
    0.8F, 0.2F, 0.3F, 1.0F });
        TinyTactics::Renderer2D::EndScene();
    }
}
