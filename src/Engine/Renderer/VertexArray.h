#pragma once

#include "Engine/Renderer/IndexBuffer.h"
#include "Engine/Renderer/VertexBuffer.h"

#include <cstdint>
#include <memory>
#include <vector>

namespace TinyTactics
{
    // Owns one OpenGL vertex array object and the buffers attached to it.
    // Flow: Layer/Renderer2D -> VertexArray::Bind -> RenderCommand::DrawIndexed.
    class VertexArray
    {
    public:
        VertexArray();
        ~VertexArray();

        VertexArray(const VertexArray&) = delete;
        VertexArray& operator=(const VertexArray&) = delete;

        VertexArray(VertexArray&& other) noexcept;
        VertexArray& operator=(VertexArray&& other) noexcept;

        void Bind() const;
        static void Unbind();

        // Calls: reads VertexBuffer layout and creates OpenGL vertex attribute pointers.
        void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer);

        // Calls: keeps the index buffer bound to this VAO for indexed draw calls.
        void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer);

        const std::shared_ptr<IndexBuffer>& GetIndexBuffer() const;

    private:
        void Release();

    private:
        uint32_t m_RendererID = 0;
        uint32_t m_VertexBufferIndex = 0;
        std::vector<std::shared_ptr<VertexBuffer>> m_VertexBuffers;
        std::shared_ptr<IndexBuffer> m_IndexBuffer;
    };
}
