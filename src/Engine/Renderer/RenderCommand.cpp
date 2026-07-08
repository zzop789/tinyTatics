#include "Engine/Renderer/RenderCommand.h"

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
}
