#include "Engine/Renderer/VertexBuffer.h"

#include <glad/gl.h>

#include <GLFW/glfw3.h>

#include <utility>

namespace TinyTactics
{
    VertexBuffer::VertexBuffer(const float* vertices, uint32_t size)
    {
        glGenBuffers(1, &m_RendererID);
        glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
        glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
    }

    VertexBuffer::~VertexBuffer()
    {
        Release();
    }

    VertexBuffer::VertexBuffer(VertexBuffer&& other) noexcept
        : m_RendererID(std::exchange(other.m_RendererID, 0)),
          m_Layout(std::move(other.m_Layout))
    {
    }

    VertexBuffer& VertexBuffer::operator=(VertexBuffer&& other) noexcept
    {
        if (this != &other)
        {
            Release();
            m_RendererID = std::exchange(other.m_RendererID, 0);
            m_Layout = std::move(other.m_Layout);
        }

        return *this;
    }

    void VertexBuffer::Bind() const
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    }

    void VertexBuffer::Unbind()
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void VertexBuffer::SetLayout(const BufferLayout& layout)
    {
        m_Layout = layout;
    }

    const BufferLayout& VertexBuffer::GetLayout() const
    {
        return m_Layout;
    }

    void VertexBuffer::Release()
    {
        if (m_RendererID != 0)
        {
            // Flow: Layer shutdown -> VertexBuffer release while OpenGL context is alive.
            if (glfwGetCurrentContext() != nullptr)
            {
                glDeleteBuffers(1, &m_RendererID);
            }

            m_RendererID = 0;
        }
    }
}
