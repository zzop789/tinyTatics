#include "Engine/Renderer/Shader.h"

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <utility>
#include <vector>

namespace TinyTactics
{
    namespace
    {
        std::string ReadOpenGLShaderLog(uint32_t shader, int logLength)
        {
            // Calls: OpenGL writes into caller-owned storage; keep one extra byte for driver terminators.
            std::vector<char> log(static_cast<size_t>(logLength > 0 ? logLength : 1) + 1, '\0');
            int writtenLength = 0;
            glGetShaderInfoLog(shader, static_cast<int>(log.size()), &writtenLength, log.data());
            return std::string(log.data(), static_cast<size_t>(writtenLength > 0 ? writtenLength : 0));
        }

        std::string ReadOpenGLProgramLog(uint32_t program, int logLength)
        {
            // Calls: OpenGL writes into caller-owned storage; keep one extra byte for driver terminators.
            std::vector<char> log(static_cast<size_t>(logLength > 0 ? logLength : 1) + 1, '\0');
            int writtenLength = 0;
            glGetProgramInfoLog(program, static_cast<int>(log.size()), &writtenLength, log.data());
            return std::string(log.data(), static_cast<size_t>(writtenLength > 0 ? writtenLength : 0));
        }
    }

    Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath)
    {
        const std::string vertexSource = ReadFile(vertexPath);
        const std::string fragmentSource = ReadFile(fragmentPath);

        const uint32_t vertexShader = CompileShader(GL_VERTEX_SHADER, vertexSource, vertexPath);
        const uint32_t fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentSource, fragmentPath);

        m_RendererID = glCreateProgram();
        glAttachShader(m_RendererID, vertexShader);
        glAttachShader(m_RendererID, fragmentShader);
        glLinkProgram(m_RendererID);

        int success = 0;
        glGetProgramiv(m_RendererID, GL_LINK_STATUS, &success);
        if (success == GL_FALSE)
        {
            int logLength = 0;
            glGetProgramiv(m_RendererID, GL_INFO_LOG_LENGTH, &logLength);

            const std::string log = ReadOpenGLProgramLog(m_RendererID, logLength);

            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);
            Release();

            throw std::runtime_error("Failed to link shader program:\n" + log);
        }

        // Calls: compiled shader objects are no longer needed after program link.
        glDetachShader(m_RendererID, vertexShader);
        glDetachShader(m_RendererID, fragmentShader);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }

    Shader::~Shader()
    {
        Release();
    }

    Shader::Shader(Shader&& other) noexcept
        : m_RendererID(std::exchange(other.m_RendererID, 0))
    {
    }

    Shader& Shader::operator=(Shader&& other) noexcept
    {
        if (this != &other)
        {
            Release();
            m_RendererID = std::exchange(other.m_RendererID, 0);
        }

        return *this;
    }

    void Shader::Bind() const
    {
        glUseProgram(m_RendererID);
    }

    void Shader::Unbind()
    {
        glUseProgram(0);
    }

    void Shader::SetMat4(const std::string& name, const glm::mat4& value) const
    {
        glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
    }

    void Shader::SetFloat4(const std::string& name, const glm::vec4& value) const
    {
        glUniform4fv(GetUniformLocation(name), 1, glm::value_ptr(value));
    }

    uint32_t Shader::GetRendererID() const
    {
        return m_RendererID;
    }

    std::string Shader::ReadFile(const std::string& path)
    {
        std::ifstream file(path, std::ios::in | std::ios::binary);
        if (!file)
        {
            throw std::runtime_error("Failed to open shader file: " + path);
        }

        std::ostringstream contents;
        contents << file.rdbuf();
        return contents.str();
    }

    uint32_t Shader::CompileShader(uint32_t type, const std::string& source, const std::string& path)
    {
        const uint32_t shader = glCreateShader(type);
        const char* sourceData = source.c_str();

        glShaderSource(shader, 1, &sourceData, nullptr);
        glCompileShader(shader);

        int success = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (success == GL_FALSE)
        {
            int logLength = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);

            const std::string log = ReadOpenGLShaderLog(shader, logLength);

            glDeleteShader(shader);
            throw std::runtime_error("Failed to compile shader '" + path + "':\n" + log);
        }

        return shader;
    }

    int Shader::GetUniformLocation(const std::string& name) const
    {
        return glGetUniformLocation(m_RendererID, name.c_str());
    }

    void Shader::Release()
    {
        if (m_RendererID != 0)
        {
            // Flow: Layer destruction -> Shader::Release -> OpenGL program delete while context is current.
            if (glfwGetCurrentContext() != nullptr)
            {
                glDeleteProgram(m_RendererID);
            }

            m_RendererID = 0;
        }
    }
}
