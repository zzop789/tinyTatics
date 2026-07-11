#include "Engine/Renderer/BufferLayout.h"

#include <stdexcept>
#include <utility>

namespace TinyTactics
{
    uint32_t GetShaderDataTypeSize(ShaderDataType type)
    {
        switch (type)
        {
            case ShaderDataType::Float:  return 4;
            case ShaderDataType::Float2: return 4 * 2;
            case ShaderDataType::Float3: return 4 * 3;
            case ShaderDataType::Float4: return 4 * 4;
            case ShaderDataType::None:
            default:
                throw std::runtime_error("Unknown ShaderDataType size.");
        }
    }

    uint32_t GetShaderDataTypeComponentCount(ShaderDataType type)
    {
        switch (type)
        {
            case ShaderDataType::Float:  return 1;
            case ShaderDataType::Float2: return 2;
            case ShaderDataType::Float3: return 3;
            case ShaderDataType::Float4: return 4;
            case ShaderDataType::None:
            default:
                throw std::runtime_error("Unknown ShaderDataType component count.");
        }
    }

    BufferElement::BufferElement(ShaderDataType elementType, std::string elementName, bool isNormalized)
        : name(std::move(elementName)),
          type(elementType),
          size(GetShaderDataTypeSize(elementType)),
          normalized(isNormalized)
    {
    }

    BufferLayout::BufferLayout(std::initializer_list<BufferElement> elements)
        : m_Elements(elements)
    {
        CalculateOffsetsAndStride();
    }

    uint32_t BufferLayout::GetStride() const
    {
        return m_Stride;
    }

    const std::vector<BufferElement>& BufferLayout::GetElements() const
    {
        return m_Elements;
    }

    void BufferLayout::CalculateOffsetsAndStride()
    {
        uint32_t offset = 0;
        m_Stride = 0;

        for (BufferElement& element : m_Elements)
        {
            element.offset = offset;
            offset += element.size;
            m_Stride += element.size;
        }
    }
}
