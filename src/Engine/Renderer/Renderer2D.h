#pragma once

#include "Engine/Renderer/Camera2D.h"

#include <glm/glm.hpp>

namespace TinyTactics
{
    // Owns the public-facing behavior for Renderer2D.
    // Flow: caller -> Renderer2D -> next subsystem.
    // Jump: add implementation details in Renderer2D.cpp.
    class Renderer2D
    {
    public:
        static void Init();
        static void Shutdown();
        static void BeginScene(const Camera2D& camera);
        static void EndScene();
        static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
    };
}