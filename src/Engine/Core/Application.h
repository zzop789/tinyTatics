#pragma once

#include "Engine/Core/LayerStack.h"
#include "Engine/Core/TimeStep.h"
#include "Engine/Core/Window.h"
#include "Engine/Renderer/Renderer2D.h"

#include <chrono>
#include <memory>
#include <string>

namespace TinyTactics
{
    // Data passed from app layer into the engine bootstrap.
    struct ApplicationSpecification
    {
        std::string name = "TinyTactics";
        uint32_t width = 1280;
        uint32_t height = 720;
        bool enableVSync = true;
        float autoCloseSeconds = 0.0F;
    };

    // Engine entry object.
    // Flow: Sandbox main -> Application -> Window + RenderCommand.
    class Application
    {
    public:
        explicit Application(const ApplicationSpecification& specification = {});
        ~Application();

        Application(const Application&) = delete;
        Application& operator=(const Application&) = delete;

        // Runs the frame loop until the window requests close.
        void Run();
        void Close();
        float GetFPS() const { return m_FPS; }

        // Flow: Sandbox/game code -> Application -> LayerStack ownership.
        Layer& PushLayer(std::unique_ptr<Layer> layer);

        // Jump: use this to reach platform/window state from future layers.
        Window& GetWindow();
        const Window& GetWindow() const;

    private:
        ApplicationSpecification m_Specification;
        std::unique_ptr<Window> m_Window;
        LayerStack m_LayerStack;
        TimeStep::Clock::time_point m_StartTime;
        TimeStep::Clock::time_point m_LastFrameTime;
        bool m_IsRunning = true;
        float m_FPS = 0.0F;
    };
}
