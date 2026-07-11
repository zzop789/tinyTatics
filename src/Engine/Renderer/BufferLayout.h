#pragma once

#include <cstdint>
#include <initializer_list>
#include <string>
#include <vector>

namespace TinyTactics
{
    enum class ShaderDataType
    {
        None = 0,
        Float,
        Float2,
        Float3,
        Float4,
    };

    uint32_t GetShaderDataTypeSize(ShaderDataType type);
    uint32_t GetShaderDataTypeComponentCount(ShaderDataType type);

    // Describes one vertex attribute inside a vertex buffer.
    // Flow: BufferLayout -> VertexArray::AddVertexBuffer -> glVertexAttribPointer.
    struct BufferElement
    {
        std::string name;
        ShaderDataType type = ShaderDataType::None;
        uint32_t size = 0;
        uint32_t offset = 0;
        bool normalized = false;

        BufferElement() = default;
        BufferElement(ShaderDataType elementType, std::string elementName, bool isNormalized = false);
    };

    // Describes the full memory layout of one vertex.
    // Jump: VertexBuffer stores this so VertexArray can configure OpenGL attributes.
    class BufferLayout
    {
    public:
        BufferLayout() = default;
        BufferLayout(std::initializer_list<BufferElement> elements);

        uint32_t GetStride() const;
        const std::vector<BufferElement>& GetElements() const;

        std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
        std::vector<BufferElement>::iterator end() { return m_Elements.end(); }
        std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
        std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }

    private:
        void CalculateOffsetsAndStride();

    private:
        std::vector<BufferElement> m_Elements;
        uint32_t m_Stride = 0;
    };
}
