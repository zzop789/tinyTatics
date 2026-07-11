#include "Engine/Renderer/IndexBuffer.h"

#include <glad/gl.h>

#include <GLFW/glfw3.h>

#include <utility>

namespace TinyTactics
{
    IndexBuffer::IndexBuffer(const uint32_t* indices, uint32_t count)
        : m_Count(count)
    {
        glGenBuffers(1, &m_RendererID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
    }

    IndexBuffer::~IndexBuffer()
    {
        Release();
    }

    IndexBuffer::IndexBuffer(IndexBuffer&& other) noexcept
        : m_RendererID(std::exchange(other.m_RendererID, 0)),
          m_Count(std::exchange(other.m_Count, 0))
    {
    }

    IndexBuffer& IndexBuffer::operator=(IndexBuffer&& other) noexcept
    {
        if (this != &other)
        {
            Release();
            m_RendererID = std::exchange(other.m_RendererID, 0);
            m_Count = std::exchange(other.m_Count, 0);
        }

        return *this;
    }

    void IndexBuffer::Bind() const
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
    }

    void IndexBuffer::Unbind()
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    uint32_t IndexBuffer::GetCount() const
    {
        return m_Count;
    }

    void IndexBuffer::Release()
    {
        if (m_RendererID != 0)
        {
            // Flow: Layer shutdown -> IndexBuffer release while OpenGL context is alive.
            if (glfwGetCurrentContext() != nullptr)
            {
                glDeleteBuffers(1, &m_RendererID);
            }

            m_RendererID = 0;
            m_Count = 0;
        }
    }
}
