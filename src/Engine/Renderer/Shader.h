#pragma once

#include <glm/glm.hpp>

#include <cstdint>
#include <string>

namespace TinyTactics
{
    // Owns one OpenGL shader program compiled from vertex/fragment GLSL files.
    // Flow: Sandbox/Renderer2D -> Shader::Bind -> OpenGL program state.
    class Shader
    {
    public:
        Shader(const std::string& vertexPath, const std::string& fragmentPath);
        ~Shader();

        Shader(const Shader&) = delete;
        Shader& operator=(const Shader&) = delete;

        Shader(Shader&& other) noexcept;
        Shader& operator=(Shader&& other) noexcept;

        // Calls: OpenGL uses this program for following draw calls.
        void Bind() const;
        static void Unbind();

        // Calls: set uniforms after Bind.
        void SetMat4(const std::string& name, const glm::mat4& value) const;
        void SetFloat4(const std::string& name, const glm::vec4& value) const;

        uint32_t GetRendererID() const;

    private:
        static std::string ReadFile(const std::string& path);
        static uint32_t CompileShader(uint32_t type, const std::string& source, const std::string& path);

        int GetUniformLocation(const std::string& name) const;
        void Release();

    private:
        uint32_t m_RendererID = 0;
    };
}