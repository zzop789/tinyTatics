#include "Engine/Core/Window.h"

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <stdexcept>

namespace TinyTactics
{
    namespace
    {
        // Shared GLFW/GLAD lifetime state for all engine windows.
        uint32_t s_GLFWWindowCount = 0;
        bool s_GLADInitialized = false;
    }

    std::unique_ptr<Window> Window::Create(const WindowSpecification& specification)
    {
        return std::make_unique<Window>(specification);
    }

    Window::Window(const WindowSpecification& specification)
        : m_Specification(specification)
    {
        Initialize();
    }

    Window::~Window()
    {
        Shutdown();
    }

    void Window::Initialize()
    {
        // First window owns GLFW startup; final window owns glfwTerminate.
        if (s_GLFWWindowCount == 0 && glfwInit() == GLFW_FALSE)
        {
            throw std::runtime_error("Failed to initialize GLFW.");
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#if defined(__APPLE__)
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#endif

        m_Handle = glfwCreateWindow(
            static_cast<int>(m_Specification.width),
            static_cast<int>(m_Specification.height),
            m_Specification.title.c_str(),
            nullptr,
            nullptr);

        if (!m_Handle)
        {
            if (s_GLFWWindowCount == 0)
            {
                glfwTerminate();
            }

            throw std::runtime_error("Failed to create GLFW window.");
        }

        ++s_GLFWWindowCount;

        // Flow: native GLFW window -> current OpenGL context -> GLAD function loading.
        glfwMakeContextCurrent(m_Handle);
        glfwSetWindowUserPointer(m_Handle, &m_Specification);

        if (!s_GLADInitialized)
        {
            if (!gladLoadGL(glfwGetProcAddress))
            {
                throw std::runtime_error("Failed to initialize GLAD.");
            }

            s_GLADInitialized = true;
        }

        glViewport(0, 0, static_cast<int>(m_Specification.width), static_cast<int>(m_Specification.height));

        // Resize callback keeps engine window size and OpenGL viewport in sync.
        glfwSetFramebufferSizeCallback(m_Handle, [](GLFWwindow* window, int width, int height)
        {
            auto* specification = static_cast<WindowSpecification*>(glfwGetWindowUserPointer(window));
            specification->width = static_cast<uint32_t>(width);
            specification->height = static_cast<uint32_t>(height);
            glViewport(0, 0, width, height);
        });

        SetVSync(m_Specification.enableVSync);
    }

    void Window::Shutdown()
    {
        if (!m_Handle)
        {
            return;
        }

        glfwDestroyWindow(m_Handle);
        m_Handle = nullptr;

        --s_GLFWWindowCount;
        if (s_GLFWWindowCount == 0)
        {
            s_GLADInitialized = false;
            glfwTerminate();
        }
    }

    void Window::OnUpdate()
    {
        // Flow: Application::Run -> Window::OnUpdate -> OS events + backbuffer present.
        glfwPollEvents();
        glfwSwapBuffers(m_Handle);
    }

    bool Window::ShouldClose() const
    {
        return glfwWindowShouldClose(m_Handle) == GLFW_TRUE;
    }

    void Window::SetVSync(bool enabled)
    {
        glfwSwapInterval(enabled ? 1 : 0);
        m_IsVSyncEnabled = enabled;
    }

    bool Window::IsVSyncEnabled() const
    {
        return m_IsVSyncEnabled;
    }

    uint32_t Window::GetWidth() const
    {
        return m_Specification.width;
    }

    uint32_t Window::GetHeight() const
    {
        return m_Specification.height;
    }

    const std::string& Window::GetTitle() const
    {
        return m_Specification.title;
    }

    GLFWwindow* Window::GetNativeHandle() const
    {
        return m_Handle;
    }
}
