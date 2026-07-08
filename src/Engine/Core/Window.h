#pragma once

#include <cstdint>
#include <memory>
#include <string>

struct GLFWwindow;

namespace TinyTactics
{
    // Window creation data. ApplicationSpecification is converted into this type.
    struct WindowSpecification
    {
        std::string title = "TinyTactics";
        uint32_t width = 1280;
        uint32_t height = 720;
        bool enableVSync = true;
    };

    // Owns the native GLFW window and OpenGL context.
    // Flow: Application -> Window::Create -> Window::Initialize -> GLAD/OpenGL context.
    class Window
    {
    public:
        static std::unique_ptr<Window> Create(const WindowSpecification& specification = {});

        explicit Window(const WindowSpecification& specification);
        ~Window();

        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;

        // Calls: glfwPollEvents + glfwSwapBuffers.
        void OnUpdate();

        bool ShouldClose() const;
        void SetVSync(bool enabled);
        bool IsVSyncEnabled() const;

        uint32_t GetWidth() const;
        uint32_t GetHeight() const;
        const std::string& GetTitle() const;

        // Jump: use this only from platform-specific code that must touch GLFW directly.
        GLFWwindow* GetNativeHandle() const;

    private:
        void Initialize();
        void Shutdown();

    private:
        WindowSpecification m_Specification;
        GLFWwindow* m_Handle = nullptr;
        bool m_IsVSyncEnabled = true;
    };
}
