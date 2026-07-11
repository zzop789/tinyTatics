#include "Engine/Renderer/VertexArray.h"

#include <glad/gl.h>

#include <GLFW/glfw3.h>

#include <stdexcept>
#include <utility>

namespace TinyTactics
{
    namespace
    {
        uint32_t ToOpenGLBaseType(ShaderDataType type)
        {
            switch (type)
            {
                case ShaderDataType::Float:
                case ShaderDataType::Float2:
                case ShaderDataType::Float3:
                case ShaderDataType::Float4:
                    return GL_FLOAT;
                case ShaderDataType::None:
                default:
                    throw std::runtime_error("Unsupported shader data type.");
            }
        }
    }

    VertexArray::VertexArray()
    {
        glGenVertexArrays(1, &m_RendererID);
    }

    VertexArray::~VertexArray()
    {
        Release();
    }

    VertexArray::VertexArray(VertexArray&& other) noexcept
        : m_RendererID(std::exchange(other.m_RendererID, 0)),
          m_VertexBufferIndex(std::exchange(other.m_VertexBufferIndex, 0)),
          m_VertexBuffers(std::move(other.m_VertexBuffers)),
          m_IndexBuffer(std::move(other.m_IndexBuffer))
    {
    }

    VertexArray& VertexArray::operator=(VertexArray&& other) noexcept
    {
        if (this != &other)
        {
            Release();
            m_RendererID = std::exchange(other.m_RendererID, 0);
            m_VertexBufferIndex = std::exchange(other.m_VertexBufferIndex, 0);
            m_VertexBuffers = std::move(other.m_VertexBuffers);
            m_IndexBuffer = std::move(other.m_IndexBuffer);
        }

        return *this;
    }

    void VertexArray::Bind() const
    {
        glBindVertexArray(m_RendererID);
    }

    void VertexArray::Unbind()
    {
        glBindVertexArray(0);
    }

    void VertexArray::AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer)
    {
        if (!vertexBuffer)
        {
            throw std::invalid_argument("VertexArray::AddVertexBuffer received null vertex buffer.");
        }

        const BufferLayout& layout = vertexBuffer->GetLayout();
        if (layout.GetElements().empty())
        {
            throw std::runtime_error("Vertex buffer has no layout.");
        }

        // Flow: VAO remembers how to interpret the currently bound VBO.
        Bind();
        vertexBuffer->Bind();

        for (const BufferElement& element : layout)
        {
            glEnableVertexAttribArray(m_VertexBufferIndex);
            glVertexAttribPointer(
                m_VertexBufferIndex,
                static_cast<int>(GetShaderDataTypeComponentCount(element.type)),
                ToOpenGLBaseType(element.type),
                element.normalized ? GL_TRUE : GL_FALSE,
                static_cast<int>(layout.GetStride()),
                reinterpret_cast<const void*>(static_cast<uintptr_t>(element.offset)));

            ++m_VertexBufferIndex;
        }

        m_VertexBuffers.push_back(vertexBuffer);
    }

    void VertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer)
    {
        if (!indexBuffer)
        {
            throw std::invalid_argument("VertexArray::SetIndexBuffer received null index buffer.");
        }

        // Calls: EBO binding is stored inside the currently bound VAO.
        Bind();
        indexBuffer->Bind();
        m_IndexBuffer = indexBuffer;
    }

    const std::shared_ptr<IndexBuffer>& VertexArray::GetIndexBuffer() const
    {
        return m_IndexBuffer;
    }

    void VertexArray::Release()
    {
        if (m_RendererID != 0)
        {
            // Flow: Layer shutdown -> VertexArray release while OpenGL context is alive.
            if (glfwGetCurrentContext() != nullptr)
            {
                glDeleteVertexArrays(1, &m_RendererID);
            }

            m_RendererID = 0;
        }
    }
}
