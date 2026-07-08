#include "Engine/Core/Application.h"

#include "Engine/Renderer/RenderCommand.h"

#include <utility>

namespace TinyTactics
{
    Application::Application(const ApplicationSpecification& specification)
        : m_Specification(specification),
          m_StartTime(TimeStep::Clock::now()),
          m_LastFrameTime(m_StartTime)
    {
        // Flow: ApplicationSpecification -> WindowSpecification -> Window::Create.
        WindowSpecification windowSpecification;
        windowSpecification.title = m_Specification.name;
        windowSpecification.width = m_Specification.width;
        windowSpecification.height = m_Specification.height;
        windowSpecification.enableVSync = m_Specification.enableVSync;

        m_Window = Window::Create(windowSpecification);

        RenderCommand::Init();
        RenderCommand::SetClearColor(0.08F, 0.10F, 0.12F, 1.0F);
    }

    Application::~Application() = default;

    void Application::Run()
    {
        while (m_IsRunning && !m_Window->ShouldClose())
        {
            const auto currentFrameTime = TimeStep::Clock::now();
            const TimeStep timestep(
                currentFrameTime - m_LastFrameTime,
                currentFrameTime - m_StartTime);
            m_LastFrameTime = currentFrameTime;
            m_FPS = 1.0F / timestep.GetSeconds();
            // Flow: frame clock -> clear -> layers update/draw -> window presents frame.
            RenderCommand::Clear();

            for (const auto& layer : m_LayerStack)
            {
                layer->OnUpdate(timestep);
            }

            m_Window->OnUpdate();
        }
    }

    void Application::Close()
    {
        m_IsRunning = false;
    }

    Layer& Application::PushLayer(std::unique_ptr<Layer> layer)
    {
        // Calls: Application keeps ownership centralized in LayerStack.
        return m_LayerStack.PushLayer(std::move(layer));
    }

    Window& Application::GetWindow()
    {
        return *m_Window;
    }

    const Window& Application::GetWindow() const
    {
        return *m_Window;
    }
}
