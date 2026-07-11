#include "Engine/Renderer/RenderCommand.h"

#include "Engine/Renderer/VertexArray.h"

#include <glad/gl.h>

namespace TinyTactics
{
    void RenderCommand::Init()
    {
        // Base 2D state: alpha blending on, depth test off.
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_DEPTH_TEST);
    }

    void RenderCommand::SetClearColor(float red, float green, float blue, float alpha)
    {
        glClearColor(red, green, blue, alpha);
    }

    void RenderCommand::Clear()
    {
        glClear(GL_COLOR_BUFFER_BIT);
    }

    void RenderCommand::DrawIndexed(const VertexArray& vertexArray)
    {
        vertexArray.Bind();
        const auto& indexBuffer = vertexArray.GetIndexBuffer();
        glDrawElements(GL_TRIANGLES, static_cast<int>(indexBuffer->GetCount()), GL_UNSIGNED_INT, nullptr);
    }
}
