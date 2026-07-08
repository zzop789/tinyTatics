#include "Engine/Core/Layer.h"

#include <utility>

namespace TinyTactics
{
    Layer::Layer(std::string debugName)
        : m_DebugName(std::move(debugName))
    {
    }

    Layer::~Layer() = default;

    const std::string& Layer::GetDebugName() const
    {
        return m_DebugName;
    }
}
