#pragma once

namespace TinyTactics
{
    // Minimal render command facade.
    // Flow: Application frame loop -> RenderCommand -> OpenGL calls.
    class RenderCommand
    {
    public:
        static void Init();
        static void SetClearColor(float red, float green, float blue, float alpha);
        static void Clear();
    };
}
