#pragma once

#include "Engine/Renderer/BufferLayout.h"

#include <cstdint>

namespace TinyTactics
{
    // Owns one OpenGL vertex buffer object.
    // Flow: BaseLayer/Renderer2D -> VertexBuffer -> VertexArray attribute setup.
    class VertexBuffer
    {
    public:
        VertexBuffer(const float* vertices, uint32_t size);
        ~VertexBuffer();

        VertexBuffer(const VertexBuffer&) = delete;
        VertexBuffer& operator=(const VertexBuffer&) = delete;

        VertexBuffer(VertexBuffer&& other) noexcept;
        VertexBuffer& operator=(VertexBuffer&& other) noexcept;

        void Bind() const;
        static void Unbind();

        void SetLayout(const BufferLayout& layout);
        const BufferLayout& GetLayout() const;

    private:
        void Release();

    private:
        uint32_t m_RendererID = 0;
        BufferLayout m_Layout;
    };
}
