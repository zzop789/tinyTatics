#include"Engine/Renderer/Camera2D.h"

#include<algorithm>
#include<glm/gtc/matrix_transform.hpp>

namespace TinyTactics
{
    Camera2D::Camera2D()
    {
        Recalculate();
    }

    void Camera2D::SetViewportSize(float width, float height)
    {
        m_ViewportWidth = width;
        m_ViewportHeight = height;
        Recalculate();
    }

    void Camera2D::SetPosition(const glm::vec2& position)
    {
        m_Position = position;
        Recalculate();
    }

    const glm::vec2& Camera2D::GetPosition() const
    {
        return m_Position;
    }

    void Camera2D::SetZoom(float zoom)
    {
        m_Zoom = std::max(zoom, 0.1f);
        Recalculate();
    }

    float Camera2D::GetZoom() const
    {
        return m_Zoom;
    }

    const glm::mat4& Camera2D::GetViewProjectionMatrix() const
    {
        return m_ViewProjection;
    }

    void Camera2D::Recalculate(){
        const float aspectRatio = m_ViewportWidth / m_ViewportHeight;
        const float halfHeight = 1.0f / m_Zoom;
        const float halfWidth = halfHeight * aspectRatio;

        m_Projection = glm::ortho(
            -halfWidth,
            halfWidth,
            -halfHeight,
            halfHeight,
            -1.0f,
            1.0f
        );

        //Camera move right means the world move left.

        m_View = glm::translate(glm::mat4(1.0f), glm::vec3(-m_Position, 0.0f));
        m_ViewProjection = m_Projection * m_View;
    }
}