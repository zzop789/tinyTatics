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
        const float vertices[] = {
            // position             // color
            -0.5F, -0.5F, 0.0F,     1.0F, 0.2F, 0.2F, 1.0F,
             0.5F, -0.5F, 0.0F,     0.2F, 1.0F, 0.2F, 1.0F,
             0.5F,  0.5F, 0.0F,     0.2F, 0.2F, 1.0F, 1.0F,
            -0.5F,  0.5F, 0.0F,     1.0F, 1.0F, 1.0F, 1.0F,
        };

        const uint32_t indices[] = {
            0, 1, 2,
            2, 3, 0,
        };

        m_VertexArray = std::make_shared<TinyTactics::VertexArray>();

        auto vertexBuffer = std::make_shared<TinyTactics::VertexBuffer>(
            vertices,
            static_cast<uint32_t>(sizeof(vertices)));
        vertexBuffer->SetLayout({
            {TinyTactics::ShaderDataType::Float3, "a_Position"},
            {TinyTactics::ShaderDataType::Float4, "a_Color"},
        });

        auto indexBuffer = std::make_shared<TinyTactics::IndexBuffer>(indices, 6);

        // Flow: Vertex data -> VertexArray layout -> RenderCommand::DrawIndexed.
        m_VertexArray->AddVertexBuffer(vertexBuffer);
        m_VertexArray->SetIndexBuffer(indexBuffer);

        m_Shader = std::make_unique<TinyTactics::Shader>("Assets/Shaders/FlatColor.vert", "Assets/Shaders/FlatColor.frag");
    }

    void BaseLayer::OnDetach()
    {
        // Flow: Application shutdown -> LayerStack::OnDetach -> release OpenGL-owned resources.
        m_VertexArray.reset();
        m_Shader.reset();
    }

    void BaseLayer::OnUpdate(TinyTactics::TimeStep timestep)
    {
        // Flow: Application::Run -> BaseLayer::OnUpdate -> Shader + VAO -> indexed draw.
        (void)timestep;

        m_Shader->Bind();
        m_Shader->SetMat4("u_ViewProjection", m_Camera.GetViewProjectionMatrix());
        TinyTactics::RenderCommand::DrawIndexed(*m_VertexArray);
    }
}
