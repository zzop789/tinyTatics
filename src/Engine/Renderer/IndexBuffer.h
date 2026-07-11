#pragma once

#include <cstdint>

namespace TinyTactics
{
    // Owns one OpenGL element/index buffer object.
    // Flow: VertexArray::SetIndexBuffer -> RenderCommand::DrawIndexed.
    class IndexBuffer
    {
    public:
        IndexBuffer(const uint32_t* indices, uint32_t count);
        ~IndexBuffer();

        IndexBuffer(const IndexBuffer&) = delete;
        IndexBuffer& operator=(const IndexBuffer&) = delete;

        IndexBuffer(IndexBuffer&& other) noexcept;
        IndexBuffer& operator=(IndexBuffer&& other) noexcept;

        void Bind() const;
        static void Unbind();

        uint32_t GetCount() const;

    private:
        void Release();

    private:
        uint32_t m_RendererID = 0;
        uint32_t m_Count = 0;
    };
}
