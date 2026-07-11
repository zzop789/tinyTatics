#pragma once
#include<glm/glm.hpp>

namespace TinyTactics
{
    // 2D camera with orthographic projection.
    // Flow: Application::Run -> Camera2D::GetViewProjectionMatrix -> Shader::SetMat4.
    class Camera2D
    {
    public:
        Camera2D();

        //calls:update this when the whindow size changes
        void SetViewportSize(float width, float height);

        void SetPosition(const glm::vec2& position);
        const glm::vec2& GetPosition() const;

        //Zoom >1 means closer;Zoom <1 means farther away.
        void SetZoom(float zoom);
        float GetZoom() const;

        const glm::mat4& GetViewProjectionMatrix() const;
    private:
        void Recalculate();
        // params below

        float m_ViewportWidth = 1280.0f;
        float m_ViewportHeight = 720.0f;
        float m_Zoom = 1.0f;

        glm::vec2 m_Position = { 0.0f, 0.0f };

        glm::mat4 m_Projection = glm::mat4(1.0f);
        glm::mat4 m_View = glm::mat4(1.0f);
        glm::mat4 m_ViewProjection = glm::mat4(1.0f);

    };
}