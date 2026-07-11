#pragma once

namespace TinyTactics
{
    class VertexArray;

    // Minimal render command facade.
    // Flow: Application frame loop -> RenderCommand -> OpenGL calls.
    class RenderCommand
    {
    public:
        static void Init();
        static void SetClearColor(float red, float green, float blue, float alpha);
        static void Clear();

        // Calls: bound shader + vertex array -> glDrawElements.
        static void DrawIndexed(const VertexArray& vertexArray);
    };
}
